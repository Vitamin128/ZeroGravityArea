import sys
import os
import json
import io
from http.server import BaseHTTPRequestHandler, HTTPServer
from google import genai

# 强制设置系统输出编码为 UTF-8，防止在处理中文路径时报错
if sys.stdout.encoding != 'utf-8':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
if sys.stderr.encoding != 'utf-8':
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8')

# Configure the API key
client = genai.Client(api_key="AIzaSyCKgOSi2CfbXAGeqJpr_xlMe6ZEwIyjHJg")

class PDFParserHandler(BaseHTTPRequestHandler):
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

            sample_file = None
            try:
                # 以二进制模式打开文件，避免 SDK 内部处理路径字符串时的编码问题
                with open(pdf_path, 'rb') as f:
                    # 上传文件，明确指定 MIME 类型
                    sample_file = client.files.upload(file=f, config={'mime_type': 'application/pdf'})
                
                # 增加重试机制处理 503 服务器繁忙
                import time
                max_retries = 3
                response = None
                for i in range(max_retries):
                    try:
                        # Use gemini-2.5-flash
                        response = client.models.generate_content(
                            model='gemini-2.5-flash',
                            contents=[sample_file, "Analyze this PDF file. Generate a concise title summarizing the content and wrap it in <title> and </title> tags. Then, extract or summarize the main body of the text and wrap it in <content> and </content> tags. Return ONLY the tags and their contents, with no asterisks and no markdown format."]
                        )
                        break  # 成功则跳出循环
                    except Exception as e:
                        # 如果是 503 错误且未达到最大重试次数，则等待后重试
                        if ("503" in str(e) or "quota" in str(e).lower()) and i < max_retries - 1:
                            wait_time = (i + 1) * 5
                            print(f"Gemini API busy or quota limited, retrying in {wait_time}s (Attempt {i+1}/{max_retries})...")
                            time.sleep(wait_time)
                            continue
                        else:
                            raise e  # 抛出异常由外层 catch 处理
                
                if response:
                    self.send_response(200)
                    self.send_header('Content-type', 'text/plain; charset=utf-8')
                    self.end_headers()
                    self.wfile.write(response.text.encode('utf-8'))
                else:
                    raise Exception("Max retries reached or no response received")
                
            except Exception as e:
                self.send_response(500)
                self.end_headers()
                self.wfile.write(f"Error parsing PDF: {e}".encode('utf-8'))
            finally:
                if sample_file:
                    try:
                        client.files.delete(name=sample_file.name)
                    except:
                        pass

        except json.JSONDecodeError:
            self.send_response(400)
            self.end_headers()
            self.wfile.write(b"Error: Invalid JSON")

def run(port=8080):
    server_address = ('127.0.0.1', port)
    httpd = HTTPServer(server_address, PDFParserHandler)
    print(f"Starting persistent PDF Parser Service on http://127.0.0.1:{port} ...")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        httpd.server_close()
        print("Service stopped.")

if __name__ == "__main__":
    run()
