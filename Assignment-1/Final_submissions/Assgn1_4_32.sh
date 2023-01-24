#!/bin/bash
while read line
do
  if grep -q "\(^\| \)$2\($\| \)"<<<"$line"
  then 
    echo "$line" | sed -r "s/([a-zA-Z])([^a-zA-Z]*)$/\u\1\E\2/; s/([a-zA-Z])([^a-zA-Z]*)([a-zA-Z])/\u\1\E\2\l\3/g"
  else 
    echo "$line" 
  fi
done < $1
