#!/bin/bash
body=""
if [ -n "$CONTENT_LENGTH" ] && [ "$CONTENT_LENGTH" -gt 0 ]; then
    body=$(head -c "$CONTENT_LENGTH")
fi

echo "Content-Type: text/html"
echo ""
echo "<html><body>"
echo "<h1>CGI OK (bash)</h1>"
echo "<p>Method: $REQUEST_METHOD</p>"
echo "<p>Query: $QUERY_STRING</p>"
echo "<p>Body: $body</p>"
echo "</body></html>"
