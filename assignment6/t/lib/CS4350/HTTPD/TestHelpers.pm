package CS4350::HTTPD::TestHelpers;

use strict;
use warnings;

use IO::Handle;
use IPC::Open2;
use Test::More;

our @ISA = qw{Exporter};
our @EXPORT = qw{run_get is_valid_response hr_code hr_header hr_body};

sub run_get($);
sub hr_code($);
sub hr_header($$);
sub hr_body($);

sub run_get($)
{
    my $path = shift;
    my $pid = open2(*RES, *REQ, './httpd', 't/docs');
    return undef unless $pid;
    print REQ "GET $path HTTP/1.0\r\n";
    print REQ "Host: localhost:4080\r\n";
    print REQ "\r\n";
    REQ->flush();
    my $response = '';
    while (my $line = <RES>) {
        $response .= $line;
    }
    close REQ;
    close RES;
    waitpid $pid, 0;
    return $response;
}

sub is_valid_response($)
{
    my $res = shift;
    like($res, qr{
            ^HTTP/1\.[01]\s+(\d+)[^\r\n]+\r?\n
            ((?:[a-zA-Z-]+:[^\r\n]+\r?\n)*)
            \r?\n}xs, 'request has proper format');
}

sub hr_code($)
{
    my $response = shift;
    if ($response =~ m/^HTTP\/1\.[01]\s+(\d+)/) {
        return $1;
    } else {
        return undef;
    }
}

sub hr_header($$)
{
    my @lines = split /\r?\n/, shift;
    my $hdr = lc shift;
    shift @lines;
    for my $l (@lines) {
        if ($l =~ m/^\s*$/) {
            return undef;
        } elsif ($l =~ m/^([a-zA-Z-]+):\s*(.+?)\s*$/) {
            if (lc $1 eq $hdr) {
                return $2;
            }
        } else {
            warn "#invalid line $l\n";
        }
    }
    return undef;
}

sub hr_body($)
{
    my $resp = shift;
    $resp =~ s/^.*?\r?\n\r?\n//s;
    return $resp;
}
