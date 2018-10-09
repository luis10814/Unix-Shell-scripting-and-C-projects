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
    ./pc-missing base vim >results
    test -f results
    test ! -s results
}

##test base set is missing 3 packages in vim
base_missing_3()
{
    export_set base vim
    ./pc-missing vim base >results
    test -s results
    test "$(wc -l <results)" -eq 3
    grep '^vim-minimal$' results
}

##test www has everything in base
www_contains_base()
{
    export_set base www
    ./pc-missing base www >results
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
    ./pc-missing www sonar >results
    test -s results
    grep '^yum-cron$' results >/dev/null
    grep -v '^bash$' results >/dev/null
    grep -v '^sssd$' results >/dev/null
    msg 'testing line count...'
    test_eq 189 "$(wc -l <results)"
}

##test sonar vs. www
sonar_vs_www()
{
    export_set www sonar
    ./pc-missing sonar www >results
    test -s results
    grep -v '^yum-cron$' results >/dev/null
    grep -v '^bash$' results >/dev/null
    grep '^sssd$' results >/dev/null
    msg 'testing line count...'
    test_eq 181 "$(wc -l <results)"
}

run_tests

# vim:ft=sh:
