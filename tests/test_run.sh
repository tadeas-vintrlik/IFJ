#!/bin/sh
echo "$0"
for file in $(ls -A)
do
    if [ -x "$file" ] && [ "$file" != "test_run.sh" ]
    then
        echo "=== Running $file ==="
        ./"$file"
        echo "=== End of $file ==="
    fi
done
