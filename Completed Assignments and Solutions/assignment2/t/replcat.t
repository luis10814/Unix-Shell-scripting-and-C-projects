#!/bin/sh

tdir="$(dirname "$0")"
. "$tdir/harness.sh"

use_files replcat wumpus.txt

##test Basic substitution of text
basic_subst()
{
    ./replcat world planet wumpus.txt >planet.txt
    if ! grep planet planet.txt; then
        false
    fi
    test 2 -eq $(grep -c planet <planet.txt)
}

##test Leave text alone
unchange_if_unfound()
{
    ./replcat apple orange wumpus.txt >orange.txt
    cmp wumpus.txt orange.txt
}

##test Read standard input
basic_subst_stdin()
{
    ./replcat world planet <wumpus.txt >planet.txt
    test 2 -eq $(grep -c planet <planet.txt)
}

run_tests
# vim:ft=sh:
