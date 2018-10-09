#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 11;
use CS4350::HTTPD::TestHelpers;
use File::stat;
use HTTP::Date;

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
ok(defined $clen, 'have content length');
cmp_ok($clen, '==', 14, 'content-length is correct');

my $stat = stat('t/docs/hello.txt');

my $lmod = hr_header($res, 'Last-Modified');
ok(defined $lmod, 'have Last-Modified header');
my $lmtime = HTTP::Date::str2time($lmod);
ok(defined $lmtime, 'Last-Modified was parseable');
cmp_ok($lmtime, '==', $stat->mtime, 'last-modified time was correct');
