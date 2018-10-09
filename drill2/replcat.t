#!/bin/sh

tdir="$(dirname "$0")"
. "$tdir/harness.sh"

use_files replcat wumpus.txt

##test Output goes to file
output_to_file()
{
    ./replcat orange apple wumpus.txt >output.txt
    test -f output.txt
}

##test Basic substitution of text
basic_subst()
{
    # TODO write the test code
    false
}

##test Leave text alone
unchange_if_unfound()
{
    # TODO write the test code
    false
}

##test Read standard input
basic_subst_stdin()
{
    # TODO write the test code
    false
}

run_tests
# vim:ft=sh:
