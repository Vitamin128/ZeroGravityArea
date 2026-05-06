import sys
import os
import json
import io
import time
from http.server import BaseHTTPRequestHandler, HTTPServer
from openai import OpenAI
from pdfminer.high_level import extract_text

# 强制设置系统输出编码为 UTF-8，防止在处理中文路径时报错
if sys.stdout.encoding != 'utf-8':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
if sys.stderr.encoding != 'utf-8':
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

# 配置 DeepSeek 客户端（兼容 OpenAI 接口）
DEEPSEEK_API_KEY = "sk-30d7456da8944bd7a1ae89991261dba0"
client = OpenAI(
    api_key=DEEPSEEK_API_KEY,
    base_url="https://api.deepseek.com"
)

# 模型列表，按优先级排列
MODELS_TO_TRY = [
    "deepseek-chat",
    "deepseek-reasoner",
]

def extract_pdf_text(pdf_path: str) -> str:
    """使用 pdfminer 将 PDF 提取为纯文本"""
    text = extract_text(pdf_path)
    # 截断过长文本，避免超出 token 限制（约 12000 字符）
    return text[:12000] if len(text) > 12000 else text

def parse_pdf_with_deepseek(pdf_path: str) -> str:
    """调用 DeepSeek API 解析 PDF 文本内容"""
    print(f"Extracting text from: {pdf_path} ...")
    pdf_text = extract_pdf_text(pdf_path)
    if not pdf_text.strip():
        raise ValueError("PDF text extraction returned empty content")

    prompt = (
        "Analyze the following resume/document text. "
        "Generate a concise title summarizing the content and wrap it in <title> and </title> tags. "
        "Then, extract or summarize the main body of the text and wrap it in <content> and </content> tags. "
        "Return ONLY the tags and their contents, with no asterisks and no markdown format.\n\n"
        f"Document text:\n{pdf_text}"
    )

    last_error = None
    for model_name in MODELS_TO_TRY:
        print(f"  --> Trying model: {model_name} ...")
        max_retries = 3
        for attempt in range(max_retries):
            try:
                response = client.chat.completions.create(
                    model=model_name,
                    messages=[
                        {"role": "user", "content": prompt}
                    ],
                    max_tokens=2048,
                    temperature=0.3,
                )
                result = response.choices[0].message.content
                print(f"      [SUCCESS] with {model_name}")
                return result
            except Exception as e:
                last_error = e
                error_str = str(e).lower()
                if ("429" in error_str or "rate" in error_str or "quota" in error_str) and attempt < max_retries - 1:
                    wait = (attempt + 1) * 5
                    print(f"      [RATE LIMIT] {model_name}, retrying in {wait}s...")
                    time.sleep(wait)
                    continue
                elif "503" in error_str or "unavailable" in error_str:
                    print(f"      [UNAVAILABLE] {model_name}, switching model...")
                    break
                else:
                    print(f"      [ERROR] {model_name}: {e}")
                    break

    raise last_error if last_error else Exception("All DeepSeek models failed")


class PDFParserHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        # 抑制默认的 HTTP 访问日志，减少噪音
        pass

    def do_POST(self):
        content_length = int(self.headers.get('Content-Length', 0))
        post_data = self.rfile.read(content_length)

        try:
            data = json.loads(post_data.decode('utf-8'))
            pdf_path = data.get('path', '')

            if not pdf_path or not os.path.exists(pdf_path):
                self.send_response(400)
                self.end_headers()
                self.wfile.write(b"Error: Invalid path or file does not exist")
                return

            try:
                result = parse_pdf_with_deepseek(pdf_path)
                self.send_response(200)
                self.send_header('Content-type', 'text/plain; charset=utf-8')
                self.end_headers()
                self.wfile.write(result.encode('utf-8'))

            except Exception as e:
                print(f"Final error for {pdf_path}: {e}")
                self.send_response(500)
                self.end_headers()
                self.wfile.write(f"Error parsing PDF: {e}".encode('utf-8'))

        except json.JSONDecodeError:
            self.send_response(400)
            self.end_headers()
            self.wfile.write(b"Error: Invalid JSON")


def run(port=8080):
    server_address = ('127.0.0.1', port)
    httpd = HTTPServer(server_address, PDFParserHandler)
    print(f"Starting PDF Parser Service (DeepSeek) on http://127.0.0.1:{port} ...")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        httpd.server_close()
        print("Service stopped.")


if __name__ == "__main__":
    run()
