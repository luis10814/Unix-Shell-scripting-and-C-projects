#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 5;
use CS4350::HTTPD::TestHelpers;
use File::stat;

# Test a bad file
print "# Testing for missing file\n";
my $res = run_get('/no-index');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+30[14]\s+/, 'Redirect code');
my $hdr = hr_header($res, 'Location');
ok(defined $hdr, 'has Location: header');
like($hdr, qr{/no-index/$}, 'path ends with /no-index/');
is($hdr, 'http://localhost:4080/no-index/', 'full path is correct');
