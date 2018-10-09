#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 2;
use CS4350::HTTPD::TestHelpers;
use File::stat;

# Test a bad file
print "# Testing for missing file\n";
my $res = run_get('/missing');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+404\s+/, 'Error response HTTP code');
