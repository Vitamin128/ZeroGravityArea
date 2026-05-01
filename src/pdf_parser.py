import sys
from google import genai

if len(sys.argv) != 2:
    print("Usage: python pdf_parser.py <pdf_path>", file=sys.stderr)
    sys.exit(1)

pdf_path = sys.argv[1]

# Configure the API key
client = genai.Client(api_key="AIzaSyCKgOSi2CfbXAGeqJpr_xlMe6ZEwIyjHJg")

def main():
    sample_file = None
    try:
        # Upload the PDF file
        sample_file = client.files.upload(file=pdf_path)
        
        # Use gemini-2.5-flash as it supports document understanding and is fast
        response = client.models.generate_content(
            model='gemini-2.5-flash',
            contents=[sample_file, "Analyze this PDF file. Generate a concise title summarizing the content and wrap it in <title> and </title> tags. Then, extract or summarize the main body of the text and wrap it in <content> and </content> tags. Return ONLY the tags and their contents, with no asterisks and no markdown format."]
        )
        
        print(response.text)
        
    except Exception as e:
        print(f"Error parsing PDF: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        # Clean up the file from Google's servers
        if sample_file:
            try:
                client.files.delete(name=sample_file.name)
            except:
                pass

if __name__ == "__main__":
    main()
