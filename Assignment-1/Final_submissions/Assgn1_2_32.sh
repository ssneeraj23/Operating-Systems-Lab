#!/usr/bin/bash
out="validation_results.txt"
while read -r v; do
    [ -z "$v" ] && continue
    temp=0;
    if [ ${#v} -ge 5 ] && [ ${#v} -le 20 ]; then
        if grep -q '^[A-Za-z][A-Za-z0-9]*[0-9][A-Za-z0-9]*$' <<<$v; then
            while read -r f; do
                if grep -qi $f <<<$v; then
                    temp=1;break
                fi
            done <"fruits.txt"
        else
            temp=2
        fi
    else
        temp=2
    fi
    if [ $temp -eq 0 ];then echo "YES" >>$out;else echo "NO" >>$out; fi
done <"$1"