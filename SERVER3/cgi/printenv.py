#!/usr/bin/python

import os

print("Content type: text/html\n")

query_string = os.environ.get("QUERY_STRING", "")

print("<html><head></head><body>")
print(f"<h2>Hello, {query_string}! </h2>")
print("</body></html>")