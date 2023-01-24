#!/usr/bin/bash

fun() {
  while read -r v; do
    c=0
    [ -z "$v" ] && continue
    echo $v
    l=0
    while read -r line; do
      [ -z "$line" ] && continue
      l=$((l + 1))
      if [ $c -eq 1 ]; then
        echo $line
        if grep -q "^.*[']['][']" <<<$line; then
          c=0
        fi
        continue
      fi
      if grep -q '^#.*' <<<$line; then
        echo "$l $line"
      fi
      if grep -q "^['][']['].*[']['][']$" <<<$line; then
        echo "$l $line"
        continue
      fi
      if grep -q "^['][']['].*" <<<$line; then
        if [ $c -eq 0 ]; then
          echo "$l $line"
          c=1
        fi
      fi
    done <$v
  done
}

find $1 -type f -name "*.py" | fun
