# Test harness runner

TEST_DIR=
export LC_COLLATE=C

run_tests()
{
    echo 1.."$ntests"
    num=0
    for test in $test_list; do
        num=$(expr "$num" + 1)
        TEST_DIR="$(mktemp -d -t shtest.XXXXXX)"
        if [ "$?" -ne 0 ]; then
            echo "error making temporary directory" >&2
            exit 2
        fi
        (set -e; cd "$TEST_DIR"; use_files $SRC_FILES; set -x; $test) 2>"$TEST_DIR/.stderr"
        if [ "$?" -eq 0 ]; then
            echo "ok $num - $test"
        else
            echo "not ok $num - $test"
            sed -e 's/^/# /' "$TEST_DIR/.stderr"
        fi
        rm -rf "$TEST_DIR"
        unset TEST_DIR
    done
}

SRC_FILES=
use_files()
{
    for file in "$@"; do
        if [ -z "$TEST_DIR" ]; then
            SRC_FILES="$SRC_FILES $file"
        else
            cp "$srcdir/$file" "$TEST_DIR"
        fi
    done
}

msg()
{
    echo "$@" >&2
}

test_eq()
{
    if [ "$1" -ne "$2" ]; then
        msg "found $2, expected $1"
        return 1
    fi
}

srcdir="$PWD"
testfile="$0"
echo "Scanning test file $testfile" >&2

test_list=$(sed -n -e '/^##test/{N; y/\n/!/; s/.*!\(..*\)()/\1/p;}' "$testfile")
ntests=0
for test in $test_list; do
    ntests=$(expr "$ntests" + 1)
done
