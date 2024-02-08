#!/usr/bin/python

import cgi

query_string = os.environ.get('QUERY_STRING', '')

query_params = cgi.parse_qs(query_string)

parameter_value = query_params.get('parameter_name', [])

if parameter_value:
    print(f"<p>Value of parameter_name: {', '.join(parameter_value)}</p>")
else:
    print("<p>No value provided for parameter_name</p>")
	