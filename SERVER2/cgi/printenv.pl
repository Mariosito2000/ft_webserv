#!/usr/bin/perl

print"<html><body>"; 
print "<h1>CGI environment\n</h1>";
foreach (sort keys %ENV) {
   print "<br>";
   print "<b>$_</b>: $ENV{$_}<br>\n";
}

print"</body></html>";