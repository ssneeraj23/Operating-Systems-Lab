#!/usr/bin/bash
while read -r v; do
    [ -z "$v" ] && continue
    temp=0
    if [ ${#v} -ge 5 ] && [ ${#v} -le 20 ] && grep -q '^[A-Za-z][A-Za-z0-9]*[0-9][A-Za-z0-9]*$' <<<$v; then
        while read -r f; do
            if grep -qi $f <<<$v; then
                temp=1
                break
            fi
        done <"fruits.txt"
    else
        temp=2
    fi
    if [ $temp -eq 0 ]; then echo "YES" >>"validation_results.txt"; else echo "NO" >>"validation_results.txt"; fi
done <"$1"
