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

##test base set has no leaves
print_no_leaves()
{
    export_set base
    test "$(./pc-leaves base |wc -l)" -eq 0
}

##test vim set has one leaf
print_single_leaf()
{
    export_set vim
    test "$(./pc-leaves vim)" = vim-minimal
}

##test www set has many leaves
big_set_leaves()
{
    use_files t/expected.www.leaves
    export_set www
    ./pc-leaves www |sort -u >www.leaves
    test -f www.leaves
    test "$(wc -l <www.leaves)" -gt 5
    grep -v '^glibc$' www.leaves
    grep -v '^bash$' www.leaves
    grep '^mercurial' www.leaves
    join -v1 expected.www.leaves www.leaves >missing.leaves
    join -v2 expected.www.leaves www.leaves >extra.leaves
    result=true
    set +x
    if [ -s extra.leaves ]; then
        result=false
        while read leaf; do
            echo "produced unexpected leaf $leaf" >&2
        done <extra.leaves
    fi
    if [ -s missing.leaves ]; then
        result=false
        while read leaf; do
            echo "did not produce expected leaf $leaf" >&2
        done <missing.leaves
    fi
    set -x
    $result
}

run_tests

# vim:ft=sh:
