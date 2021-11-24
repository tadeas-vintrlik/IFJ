#!/bin/sh
echo "$0"
for file in $(ls -A)
do
    if [ -f "$file" ]&&[ -x "$file" ]&&[ "$file" != "test_run.sh" ]
    then
        echo "=== Running $file ==="
        ./"$file"
        RC="$?"
        echo "=== End of $file ==="
        if [ "$RC" != 0 ]
        then
            exit 1
        fi
    fi
done

exit 0
