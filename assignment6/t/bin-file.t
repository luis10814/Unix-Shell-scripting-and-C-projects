#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 7;
use CS4350::HTTPD::TestHelpers;
use File::stat;
use Digest::MD5;

# Test an HTML file
note("Testing a binary file");
my $res = run_get('/lenskit.png');
is_valid_response($res);
like($res, qr/^HTTP\/1\.[01]\s+200\s+OK\s*$/m, 'PNG successful HTTP code');
my $ctype = hr_header($res, 'content-type');
ok(!defined($ctype) || $ctype eq 'image/png', 'CSS content-type');
my $fsize = stat('t/docs/lenskit.png')->size;
cmp_ok(length(hr_body($res)), '==', $fsize, 'PNG file has correct size');

my $ctx = Digest::MD5->new;
open PNG, '<', 't/docs/lenskit.png';
$ctx->addfile(*PNG);
close PNG;
my $expected = $ctx->hexdigest;
$ctx->reset;
$ctx->add(hr_body($res));
is($ctx->hexdigest, $expected, 'content digest matches');

my $clen = hr_header($res, 'content-length');
SKIP: {
    skip 'no content length provided', 2 unless defined $clen;
    cmp_ok($clen, '==', $fsize, 'Content-length equals file size');
    cmp_ok($clen, '==', length(hr_body($res)), 'Content-length equals sent content');
}

