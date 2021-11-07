#!/bin/sh

echo "Test 1: "
./sc_tests/test_scanner1 < ./source_codes/sc_1
echo
echo "Test 2: "
./sc_tests//test_scanner2 < ./source_codes/example1.tl
echo
echo "Test 3: "
./sc_tests//test_scanner3 < ./source_codes/example2.tl
echo
echo "Error test, edit the err_exampl.tl to see recieved tokens Test 4: "
./sc_tests//test_scanner4 < ./source_codes/err_exampl.tl
echo
echo "Test 5: "
./sc_tests//test_scanner5 < ./source_codes/whitespaces.tl
