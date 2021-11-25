#!/bin/sh
EXECUTABLE="../main"
RC_OK="0"
RC_SYN_ERR="2"
RC_SEM_UNDEF_ERR="3"
TESTS="13"
SUCCESSFULL="0"

function success() {
    echo "=== Test Succeeded ==="
}

function fail() {
    echo "!!! Test FAILED !!!"
}

function check_ok() {
    if [ "$1" == "$RC_OK" ]
    then
        SUCCESSFULL=$((SUCCESSFULL + 1))
        success
    else
        echo "RC was $RC expected $RC_OK"
        fail
    fi
}

function check_syntax_err() {
    if [ "$1" == "$RC_SYN_ERR" ]
    then
        SUCCESSFULL=$((SUCCESSFULL + 1))
        success
    else
        echo "RC was $RC expected $RC_SYN_ERR"
        fail
    fi
}

function test_run() {
    echo "=== Running test $1 ==="
    "$EXECUTABLE" < "$1" >/dev/null
    RC="$?"
    if [ "$2" == "OK" ]
    then
        check_ok "$RC"
    elif [ "$2" == "SYN_ERR" ]
    then
        check_syntax_err "$RC"
    fi
}

test_run ./source_codes/example1.tl OK
test_run ./source_codes/example2.tl OK
test_run ./source_codes/example3.tl OK
test_run ./source_codes/expr.tl OK
test_run ./source_codes/fun.tl OK
test_run ./source_codes/hello.tl OK
test_run ./source_codes/multiassign.tl OK
test_run ./source_codes/sc_1 OK
test_run ./source_codes/substr.tl OK
test_run ./source_codes/visibility.tl OK
test_run ./source_codes/whitespaces.tl OK
test_run ./source_codes/syn_err1.tl SYN_ERR
test_run ./source_codes/syn_err2.tl SYN_ERR

if [ "$SUCCESSFULL" == "$TESTS" ]
then
    echo "=== All tests were successfull ==="
    exit 0
else
    echo "=== $((TESTS - SUCCESSFULL)) out of $TESTS tests FAILED. ==="
    exit 1
fi
