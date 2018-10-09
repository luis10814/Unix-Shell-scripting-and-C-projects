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

##test Test version output
basic_versions()
{
    export_set vim base
    ./pc-versions base vim |while read pkg v1 v2; do
        case "$pkg" in
            xz-libs) test "$v1" != "$v2";;
            *) test "$v1" = "$v2";;
        esac
    done
}

##test Test version output
www_sonar_versions()
{
    export_set sonar www
    ./pc-versions sonar www |while read pkg v1 v2; do
        if [ "$v1" != "$v2" ]; then
            echo "$pkg $v1 $v2"
        fi
    done >different
    grep '^basesystem' different >/dev/null
    grep '^yum' different >/dev/null
    grep -v '^bash' different >/dev/null
}

run_tests

# vim:ft=sh:
