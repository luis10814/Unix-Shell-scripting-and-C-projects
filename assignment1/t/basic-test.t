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

##test Exports package info
export_packages()
{
    export_set base
    test -f base.versions
    test -f base.deps
    test "$(grep -c '^pkg' base.mf)" -eq "$(wc -l <base.versions)"
}

##test Prints no leaf packages
print_no_leaves()
{
    export_set base
    test "$(./pc-leaves base |wc -l)" -eq 0
}

##test Prints a leaf package for a single-leaf package set
print_single_leaf()
{
    export_set vim
    test "$(./pc-leaves vim)" = vim-minimal
}

##test Handle no missing packages correctly
no_missing()
{
    echo "# pc-missing base vim should produce no output"
    export_set vim base
    test "$(./pc-missing base vim)" = ""
}

##test Find that 3 vim packages are not in base
missing_vim()
{
    echo "# pc-missing vim base should produce some output"
    export_set vim base
    test "$(./pc-missing vim base |wc -l)" -eq 3
}

##test Prints the single missing leaf of vim
print_missing_leaf()
{
    echo "# pc-missing-leaves vim base should ouptput vim-minimal"
    export_set vim base
    test "$(./pc-missing-leaves vim base)" = vim-minimal
}

##test Handle no missing leaves correctly
no_missing_leaves()
{
    echo "# pc-missing-leaves base vim should produce no output"
    export_set vim base
    test "$(./pc-missing-leaves base vim)" = ""
}

##test Test version output
versions()
{
    export_set vim base
    ./pc-versions base vim |while read pkg v1 v2; do
        case "$pkg" in
            xz-libs) test "$v1" != "$v2";;
            *) test "$v1" = "$v2";;
        esac
    done
}

run_tests

# vim:ft=sh
