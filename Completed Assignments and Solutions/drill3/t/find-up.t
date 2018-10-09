#!/bin/sh

tdir="$(dirname "$0")"

. "$tdir/harness.sh"

use_files find-up.sh

##test Script is executable
script_executable()
{
    if ! test -x find-up.sh; then
        echo "find-up not executable" >&2
        false
    fi
}

##test Usage error
usage_error()
{
    set +e
    output=$(./find-up.sh)
    code="$?"
    set -e

    test "$code" -eq 2
    test -z "$output"
}

##test No file wombat
no_wombat()
{
    if ./find-up.sh wombat >&2; then
        echo "Found nonexistant directory" >&2
        false
    fi
}

##test Finds itself
find_self()
{
    loc=$(./find-up.sh find-up.sh)
    test "$loc" = ./find-up.sh
}

##test Finds itself in parent directory
find_self_in_parent()
{
    export PATH="$(pwd):$PATH"
    mkdir foo
    loc=$( (cd foo && find-up.sh find-up.sh) )
    test "$loc" = ../find-up.sh
}

##test Search for great-grandparents
find_grandparent()
{
    export PATH="$(pwd):$PATH"
    touch needle
    mkdir -p big/hay/stack
    loc=$( (cd big/hay/stack && find-up.sh needle) )
    test "$loc" = ../../../needle
}

##test Search for multiple files
find_multiple()
{
    export PATH="$(pwd):$PATH"
    touch needle
    mkdir -p haystack
    loc=$( (cd haystack && find-up.sh pin needle) )
    test "$loc" = ../needle
}

##test Search for multiple files, find first dir
find_multiple_first_dir()
{
    export PATH="$(pwd):$PATH"
    mkdir -p big/hay/stack
    touch pin
    touch big/needle
    loc=$( (cd big/hay/stack && find-up.sh pin needle) )
    test "$loc" = ../../needle
}

run_tests

# vim:ft=sh:
