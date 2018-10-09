#!/bin/sh

. ./t/harness.sh

use_files pc-*
use_files *.mf

export_set()
{
    set +x
    for name in "$@"; do
        PKGMF="$name.mf" './pc-export-packages' "$name"
    done
    set -x
}

##test base set is subsumed in vim
no_missing()
{
    export_set base vim
    ./pc-missing-leaves base vim >results
    test -f results
    test ! -s results
}

##test base set is missing 3 packages in vim
base_missing_1()
{
    export_set base vim
    ./pc-missing-leaves vim base >results
    test -s results
    test_eq 1 "$(wc -l <results)"
    grep '^vim-minimal$' results
}

##test www has everything in base
www_contains_base()
{
    export_set base www
    ./pc-missing-leaves base www >results
    test -f results
    if [ -s results ]; then
        msg "erroneously marked packages as missing:"
        sed -e 's/^/  /' results >&2
        false
    fi
}

##test www vs. sonar
www_vs_sonar()
{
    export_set www sonar
    ./pc-missing-leaves www sonar >results
    test -s results
    grep '^yum-cron$' results >/dev/null
    grep -v '^bash$' results >/dev/null
    grep -v '^sssd$' results >/dev/null
    msg 'testing line count...'
    test_eq 39 "$(wc -l <results)"
}

##test sonar vs. www
sonar_vs_www()
{
    export_set www sonar
    ./pc-missing-leaves sonar www >results
    test -s results
    grep -v '^yum-cron$' results >/dev/null
    grep -v '^bash$' results >/dev/null
    grep '^sssd$' results >/dev/null
    msg 'testing line count...'
    test_eq 19 "$(wc -l <results)"
}

##test consistency of missing and leaves
missing_leaves_consistent()
{
    ./pc-missing sonar www |sort -u >missing.raw
    ./pc-missing-leaves sonar www |sort -u >missing.leaves
    ./pc-leaves sonar |sort -u >sonar.leaves
    join -v1 missing.leaves sonar.leaves >not.leaves
    test -f not.leaves
    if [ -s not.leaves ]; then
        msg "thought the following were missing leaves but not leaves:"
        cat not.leaves >&2
        false
    fi
    join missing.raw sonar.leaves >l2
    cmp missing.leaves l2
}

run_tests

# vim:ft=sh:
