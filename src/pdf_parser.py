import sys
import os
import json
from http.server import BaseHTTPRequestHandler, HTTPServer
from google import genai

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
                # Upload the PDF file
                sample_file = client.files.upload(file=pdf_path)
                
                # Use gemini-2.5-flash
                response = client.models.generate_content(
                    model='gemini-2.5-flash',
                    contents=[sample_file, "Analyze this PDF file. Generate a concise title summarizing the content and wrap it in <title> and </title> tags. Then, extract or summarize the main body of the text and wrap it in <content> and </content> tags. Return ONLY the tags and their contents, with no asterisks and no markdown format."]
                )
                
                self.send_response(200)
                self.send_header('Content-type', 'text/plain; charset=utf-8')
                self.end_headers()
                self.wfile.write(response.text.encode('utf-8'))
                
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
