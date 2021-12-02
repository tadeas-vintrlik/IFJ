#!/bin/sh
EXECUTABLE="../main"
RC_OK="0"
RC_LEX_ERR="1"
RC_SYN_ERR="2"
RC_SEM_UNDEF_ERR="3"
RC_SEM_ASSIGN_ERR="4"
RC_SEM_CALL_ERR="5"
RC_SEM_EXPR_ERR="6"
TESTS="0"
SUCCESSFULL="0"

function success() {
    echo "=== Test Succeeded ==="
    echo ""
}

function fail() {
    echo "!!! Test FAILED !!!"
    echo ""
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

function check_lexical_err() {
    if [ "$1" == "$RC_LEX_ERR" ]
    then
        SUCCESSFULL=$((SUCCESSFULL + 1))
        success
    else
        echo "RC was $RC expected $RC_LEX_ERR"
        fail
    fi
}

function check_semantic_undefined_err() {
    if [ "$1" == "$RC_SEM_UNDEF_ERR" ]
    then
        SUCCESSFULL=$((SUCCESSFULL + 1))
        success
    else
        echo "RC was $RC expected $RC_SEM_UNDEF_ERR"
        fail
    fi
}

function check_semantic_call_err() {
    if [ "$1" == "$RC_SEM_CALL_ERR" ]
    then
        SUCCESSFULL=$((SUCCESSFULL + 1))
        success
    else
        echo "RC was $RC expected $RC_SEM_CALL_ERR"
        fail
    fi
}

function check_semantic_expression_err() {
    if [ "$1" == "$RC_SEM_EXPR_ERR" ]
    then
        SUCCESSFULL=$((SUCCESSFULL + 1))
        success
    else
        echo "RC was $RC expected $RC_SEM_EXPR_ERR"
        fail
    fi
}

function check_semantic_assignment_err() {
    if [ "$1" == "$RC_SEM_ASSIGN_ERR" ]
    then
        SUCCESSFULL=$((SUCCESSFULL + 1))
        success
    else
        echo "RC was $RC expected $RC_SEM_ASSIGN_ERR"
        fail
    fi
}

function test_run() {
    echo "=== Running test $1 ==="
    TESTS=$((TESTS + 1))
    "$EXECUTABLE" < "$1" >/dev/null
    RC="$?"
    if [ "$2" == "OK" ]
    then
        check_ok "$RC"
    elif [ "$2" == "SYN_ERR" ]
    then
        check_syntax_err "$RC"
    elif [ "$2" == "LEX_ERR" ]
    then
        check_lexical_err "$RC"
    elif [ "$2" == "SEM_UNDEF_ERR" ]
    then
        check_semantic_undefined_err "$RC"
    elif [ "$2" == "SEM_CALL_ERR" ]
    then
	  check_semantic_call_err "$RC"
    elif [ "$2" == "SEM_EXPR_ERR" ]
    then
	  check_semantic_expression_err "$RC"
    elif [ "$2" == "SEM_ASSIGN_ERR" ]
    then
	  check_semantic_assignment_err "$RC"
    fi
}

test_run ./source_codes/example1.tl OK
test_run ./source_codes/example2.tl OK
test_run ./source_codes/example3.tl OK
test_run ./source_codes/expr.tl OK
test_run ./source_codes/expr2.tl OK
test_run ./source_codes/fun.tl OK
test_run ./source_codes/hello.tl OK
test_run ./source_codes/multiassign.tl OK
test_run ./source_codes/less_left_sides.tl OK
test_run ./source_codes/assign_num_int.tl OK
test_run ./source_codes/sc_1 OK
test_run ./source_codes/substr.tl OK
test_run ./source_codes/visibility.tl OK
test_run ./source_codes/whitespaces.tl OK
test_run ./source_codes/nil_return.tl OK
test_run ./source_codes/code_after_return.tl OK
test_run ./source_codes/less_return.tl OK
test_run ./source_codes/syn_err1.tl SYN_ERR
test_run ./source_codes/syn_err2.tl SYN_ERR
test_run ./source_codes/lex_err1.tl LEX_ERR
test_run ./source_codes/sem_err_undef1.tl SEM_UNDEF_ERR
test_run ./source_codes/sem_err_undef2.tl SEM_UNDEF_ERR
test_run ./source_codes/sem_err_undef3.tl SEM_UNDEF_ERR
test_run ./source_codes/sem_err_undef4.tl SEM_UNDEF_ERR
test_run ./source_codes/sem_err_call1.tl SEM_CALL_ERR
test_run ./source_codes/sem_err_call2.tl SEM_CALL_ERR
test_run ./source_codes/sem_err_call3.tl SEM_CALL_ERR
test_run ./source_codes/sem_err_call4.tl SEM_CALL_ERR
test_run ./source_codes/more_return.tl SEM_CALL_ERR
test_run ./source_codes/sem_err_expr1.tl SEM_EXPR_ERR
test_run ./source_codes/sem_err_expr2.tl SEM_EXPR_ERR
test_run ./source_codes/sem_err_expr3.tl SEM_EXPR_ERR
test_run ./source_codes/sem_err_expr4.tl SEM_EXPR_ERR
test_run ./source_codes/sem_err_expr5.tl SEM_EXPR_ERR
test_run ./source_codes/sem_err_assign1.tl SEM_ASSIGN_ERR
test_run ./source_codes/sem_err_assign2.tl SEM_ASSIGN_ERR
test_run ./source_codes/sem_err_assign3.tl SEM_ASSIGN_ERR

if [ "$SUCCESSFULL" == "$TESTS" ]
then
    echo "=== All tests were successfull ==="
    exit 0
else
    echo "=== $((TESTS - SUCCESSFULL)) out of $TESTS tests FAILED. ==="
    exit 1
fi
