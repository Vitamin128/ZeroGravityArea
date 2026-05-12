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

# --- 1. PDF 文本提取函数 ---
# 逻辑：利用第三方库 pdfminer 将二进制 PDF 文件转为长字符串，并进行长度截断。
def extract_pdf_text(pdf_path: str) -> str:
    """使用 pdfminer 将 PDF 提取为纯文本"""
    # 调用 pdfminer 库函数进行提取
    text = extract_text(pdf_path)
    
    # 截断逻辑：类似于 C++ 的 text.substr(0, 12000)
    # 目的：大模型(LLM)一次处理的 Token 有限，这里硬截断防止超出 API 限制
    return text[:12000] if len(text) > 12000 else text

# --- 2. 大模型解析函数 ---
# 逻辑：遍历模型列表，尝试调用 DeepSeek API 进行文本清洗和修复（修复 PDF 提取时的乱码和断行）。
def parse_pdf_with_deepseek(pdf_path: str) -> str:
    """调用 DeepSeek API 解析 PDF 文本内容"""
    print(f"Extracting text from: {pdf_path} ...")
    
    # 调用上方的提取函数
    pdf_text = extract_pdf_text(pdf_path)
    
    # 空白检查：类似 if (pdf_text.strip().empty())
    if not pdf_text.strip():
        raise ValueError("PDF text extraction returned empty content")

    # 构造 Prompt：告诉 AI 如何修复这段“混乱”的文本
    prompt = (
        "The following is a messy text extracted from a PDF. "
        "Please restore it into a smooth, continuous, and complete body of text. "
        "Remove any corrupted characters, fix broken line breaks, and maintain the original meaning. "
        "DO NOT summarize. DO NOT generate a title. DO NOT use any XML-like tags (like <title> or <content>). "
        "Return ONLY the restored plain text, with no additional comments or markdown formatting.\n\n"
        f"Messy PDF text:\n{pdf_text}"
    )

    last_error = None
    # 外层循环：遍历模型列表（deepseek-chat -> deepseek-reasoner）
    for model_name in MODELS_TO_TRY:
        print(f"  --> Trying model: {model_name} ...")
        max_retries = 3 # 每个模型最多重试 3 次
        
        # 内层循环：针对单个模型进行重试（处理频率限制 429 等错误）
        for attempt in range(max_retries):
            try:
                # 类似一个网络阻塞调用（RPC call）
                response = client.chat.completions.create(
                    model=model_name,
                    messages=[
                        {"role": "user", "content": prompt}
                    ],
                    max_tokens=2048,
                    temperature=0.3, # 降低随机性，保证恢复的文本严谨
                )
                
                # 提取 AI 返回的内容
                result = response.choices[0].message.content
                print(f"      [SUCCESS] with {model_name}")
                return result
            except Exception as e:
                # 异常处理：类似 C++ 的 catch (const std::exception& e)
                last_error = e
                error_str = str(e).lower()
                
                # 针对不同错误类型的处理策略
                # 429 或 Rate Limit：触发指数退避等待后重试
                if ("429" in error_str or "rate" in error_str or "quota" in error_str) and attempt < max_retries - 1:
                    wait = (attempt + 1) * 5
                    print(f"      [RATE LIMIT] {model_name}, retrying in {wait}s...")
                    time.sleep(wait)
                    continue
                # 服务不可用：切换下一个模型
                elif "503" in error_str or "unavailable" in error_str:
                    print(f"      [UNAVAILABLE] {model_name}, switching model...")
                    break
                # 其他严重错误：打印并切换下一个模型
                else:
                    print(f"      [ERROR] {model_name}: {e}")
                    break

    # 如果所有模型都试过了还不行，抛出异常
    raise last_error if last_error else Exception("All DeepSeek models failed")


# --- 3. HTTP 请求处理器 ---
# 逻辑：继承自 BaseHTTPRequestHandler，类似 C++ 中的 HTTP Handler 类。
# 接收包含 PDF 路径的 JSON 请求，返回解析后的文本。
class PDFParserHandler(BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        # 抑制默认的 HTTP 访问日志，减少屏幕噪音
        pass

    # 处理 POST 请求，类似于处理路由回调
    def do_POST(self):
        # 1. 读取请求体长度
        content_length = int(self.headers.get('Content-Length', 0))
        # 2. 读取二进制原始数据
        post_data = self.rfile.read(content_length)

        try:
            # 3. JSON 解析：类似 std::json::parse()
            data = json.loads(post_data.decode('utf-8'))
            pdf_path = data.get('path', '')

            # 4. 参数检查：确保文件路径存在
            if not pdf_path or not os.path.exists(pdf_path):
                self.send_response(400) # Bad Request
                self.end_headers()
                self.wfile.write(b"Error: Invalid path or file does not exist")
                return

            try:
                # 5. 调用核心解析函数
                result = parse_pdf_with_deepseek(pdf_path)
                # 6. 返回成功结果（200 OK）
                self.send_response(200)
                self.send_header('Content-type', 'text/plain; charset=utf-8')
                self.end_headers()
                self.wfile.write(result.encode('utf-8'))

            except Exception as e:
                # 业务逻辑报错处理
                print(f"Final error for {pdf_path}: {e}")
                self.send_response(500) # Internal Server Error
                self.end_headers()
                self.wfile.write(f"Error parsing PDF: {e}".encode('utf-8'))

        except json.JSONDecodeError:
            # JSON 格式错误处理
            self.send_response(400)
            self.end_headers()
            self.wfile.write(b"Error: Invalid JSON")


# --- 4. 服务启动入口 ---
# 逻辑：配置监听地址和端口，开启无限循环监听。
def run(port=8080):
    # 绑定地址：127.0.0.1 表示仅接受本地请求
    server_address = ('127.0.0.1', port)
    httpd = HTTPServer(server_address, PDFParserHandler)
    print(f"Starting PDF Parser Service (DeepSeek) on http://0.0.0.0:{port} ...")
    try:
        # 进入事件循环，类似 while(true) { accept(); }
        httpd.serve_forever()
    except KeyboardInterrupt:
        # 捕获 Ctrl+C
        pass
    finally:
        # 关闭套接字
        httpd.server_close()
        print("Service stopped.")


if __name__ == "__main__":
    run()
