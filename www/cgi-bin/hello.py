#!/opt/homebrew/bin/python3
import os
import sys

method = os.environ.get("REQUEST_METHOD", "")
query = os.environ.get("QUERY_STRING", "")
length = int(os.environ.get("CONTENT_LENGTH", "0") or "0")
body = sys.stdin.read(length) if length > 0 else ""

print("Content-Type: text/html")
print()
print("<html><body>")
print("<h1>CGI OK</h1>")
print("<p>Method: {}</p>".format(method))
print("<p>Query: {}</p>".format(query))
print("<p>Body: {}</p>".format(body))
print("</body></html>")
