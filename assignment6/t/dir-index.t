#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 6;
use CS4350::HTTPD::TestHelpers;
use File::stat;

# Test a bad file
print "# Testing for missing file\n";
my $res = run_get('/no-index/');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+200\s+/, 'response code 200');
my $ctype = hr_header($res, 'content-type');
my $body = hr_body($res);
ok(defined $ctype, "has content type");
is($ctype, 'text/html', "content type is text/html");
like($body, qr/href="hunt.txt"/, 'has link to hunt.txt');
like($body, qr/href="subdir\/?"/, 'has link to subdir');
