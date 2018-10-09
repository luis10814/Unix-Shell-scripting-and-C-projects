#!/bin/sh

. ./t/harness.sh

use_files pc-*
use_files *.mf

##test Basic export functionality
basic_export()
{
    PKGMF=base.mf ./pc-export-packages base
    test -f base.versions
    test -f base.deps
    test "$(grep -c '^pkg' base.mf)" -eq "$(wc -l <base.versions)"
}

##test No self-dependencies
no_self_deps()
{
    PKGMF=base.mf ./pc-export-packages base
    test -f base.versions
    test -f base.deps
    awk -F, '{ if ($1 == $2) exit 1 }' base.deps
}

##test Truncate output
truncate_output()
{
    PKGMF=base.mf ./pc-export-packages base
    lines=$(wc -l <base.deps)
    PKGMF=base.mf ./pc-export-packages base
    lines2=$(wc -l <base.deps)
    test "$lines" -eq "$lines2"
}

##test Exclude packages without dependencies
exclude_nodep()
{
    PKGMF=base.mf ./pc-export-packages base
    if grep '^tzdata' base.deps; then
        echo "unexpected tzdata lines" >&2
        exit 2
    fi
}

##test Include packages without dependencies in versions
include_nodep_version()
{
    PKGMF=base.mf ./pc-export-packages base
    if ! grep '^tzdata' base.versions >/dev/null; then
        echo "no tzdata package" >&2
        false
    fi
}

##test Export big package set
export_big_set()
{
    PKGMF=www.mf ./pc-export-packages www
    test -f www.versions
    test -f www.deps
    test $(wc -l <www.versions) -eq $(grep -c '^pkg' <www.mf)
    cut -d, -f1 www.versions >pkgs
    sort pkgs >pkgs.sorted
    if ! cmp pkgs pkgs.sorted; then
        echo "warning: packages not sorted" >&2
    fi
    if ! grep '^pkg' www.mf |cut -d' ' -f2 |sort |cmp - pkgs.sorted; then
        echo "package mismatch" >&2
        false
    fi
}

run_tests

# vim:ft=sh:
