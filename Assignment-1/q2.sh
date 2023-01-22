#!/usr/bin/bash

input="usernames.txt"
out="validation_results.txt"

while read -r v; do
    [ -z "$v" ] && continue
    temp=1
    size=${#v}
    if [ $size -ge 5 ] && [ $size -le 20 ]; then
        if grep -q '^[A-Za-z][A-Za-z0-9]*[0-9][A-Za-z0-9]*$' <<<$v; then
            while read -r f; do
                if grep -qi $f <<<$v; then
                    temp=0
                    break
                fi
            done <"fruits.txt"
            if [ $temp -eq 1 ];then echo "YES" >>$out;else echo "NO" >>$out;fi
        else
            echo "NO" >>$out
        fi
    else
        echo "NO" >>$out
    fi

done <"$input"
