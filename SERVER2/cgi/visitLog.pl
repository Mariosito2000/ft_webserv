#!/usr/bin/perl

use strict;
use warnings;

my $query_string = $ENV{'QUERY_STRING'};

unless ($query_string) {
    die "No query string found in CGI environment.\n";
}

my %params;
my @pairs = split(/&/, $query_string);
foreach my $pair (@pairs) {
    my ($key, $value) = split(/=/, $pair);
    $value = '' unless defined $value;
	$value =~ s/\+/ /g;
    $params{$key} = $value;
}


my ($sec, $min, $hour, $day, $month, $year) = localtime();

$year += 1900;
$month += 1;

my $formatted_time = sprintf("%04d-%02d-%02d %02d:%02d:%02d", $year, $month, $day, $hour, $min, $sec);


my $output_file = "SERVER1/cgi_upload/visitlog.txt";
open my $fh, '>>', $output_file or die "Cannot open $output_file: $!\n";

print $fh "\n------\n\n";
foreach my $key (sort keys %params) {
    print $fh "$key: $params{$key}\n";
}
print $fh "TIME: $formatted_time\n";
close $fh;

print "<html><body>Visit logged!\n</body></html>";
