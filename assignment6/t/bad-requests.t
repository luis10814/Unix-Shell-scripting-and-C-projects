#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 4;
use CS4350::HTTPD::TestHelpers;
use File::stat;

# Test a bad file
my $res = run_get('/../../../etc/passwd');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+400\s+/, '.. produces HTTP 400');

$res = run_get('file');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+400\s+/, 'non-absolute path produces 400');
