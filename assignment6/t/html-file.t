#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 7;
use CS4350::HTTPD::TestHelpers;
use File::stat;

# Test an HTML file
print "# Testing valid HTML file\n";
my $res = run_get('/index.html');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+200\s+OK\s*$/m, 'HTML successful HTTP code');
is(hr_header($res, 'content-type'), 'text/html', 'HTML content-type');
like(hr_body($res), qr/^<!doctype html>\s*<html>.*<\/html>\s*$/s, 'HTML header data');
my $fsize = stat('t/docs/index.html')->size;
cmp_ok(length(hr_body($res)), '==', $fsize, 'HTML file has correct size');
my $clen = hr_header($res, 'content-length');
SKIP: {
    skip 'no content length provided', 2 unless defined $clen;
    cmp_ok($clen, '==', $fsize, 'Content-length equals file size');
    cmp_ok($clen, '==', length(hr_body($res)), 'Content-length equals sent content');
}
