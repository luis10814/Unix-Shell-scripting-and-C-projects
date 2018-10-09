#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 7;
use CS4350::HTTPD::TestHelpers;
use File::stat;

# Test an HTML file
note("Testing a larger (few hundred KB) file");
my $res = run_get('/master.css');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+200\s+OK\s*$/m, 'HTML successful HTTP code');
my $ctype = hr_header($res, 'content-type');
ok(!defined($ctype) || $ctype eq 'text/css', 'CSS content-type');
like(hr_body($res), qr/^meta\.foundation-version/s, 'HTML header data');
my $fsize = stat('t/docs/master.css')->size;
cmp_ok(length(hr_body($res)), '==', $fsize, 'HTML file has correct size');
my $clen = hr_header($res, 'content-length');
SKIP: {
    skip 'no content length provided', 2 unless defined $clen;
    cmp_ok($clen, '==', $fsize, 'Content-length equals file size');
    cmp_ok($clen, '==', length(hr_body($res)), 'Content-length equals sent content');
}

