
from google import genai
import os

# 从 pdf_parser.py 中提取 API KEY (这里直接硬编码测试，因为刚才代码里有)
api_key = "AIzaSyCKgOSi2CfbXAGeqJpr_xlMe6ZEwIyjHJg"

try:
    client = genai.Client(api_key=api_key)
    print("Available models for your API Key:")
    for m in client.models.list():
        print(f" - {m.name}")
except Exception as e:
    print(f"Error listing models: {e}")
