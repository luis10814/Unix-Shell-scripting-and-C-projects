#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 7;
use CS4350::HTTPD::TestHelpers;
use File::stat;

##test
ok(-x 'httpd', 'httpd is executable program');

# Test a basic HTTP response.
my $res = run_get('/hello.txt');
ok(defined $res, 'Basic response received');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+200\s+OK\s*$/m, 'Basic response HTTP code');
is(hr_header($res, 'content-type'), 'text/plain', 'Basic response HTTP content-type');
my $clen = hr_header($res, 'content-length');
is(hr_body($res), "Hello, world.\n", 'Basic response HTTP content-type');
SKIP: {
    skip 'no content length', 1 unless defined $clen;
    cmp_ok($clen, '==', 14);
}
