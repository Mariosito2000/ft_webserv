#!/usr/bin/env python3

import cgi, cgitb, os, sys

form = cgi.FieldStorage() 

first_name = form.getvalue('name')
hometown = form.getvalue('hometown')
day = form.getvalue('day')

print(f"<h1>Hi, {first_name} from {hometown}!\n</h1>")
print(f"<h1>Happy {day}!</h1>")
