#!/bin/bash

nums=($(seq 2 1000))

for i in ${nums[@]}; do
prime[$i]=1
done

for i in ${nums[@]}; do
    if [[ ${prime[$i]} -ne 0 ]]; then
        for (( c=$((i*2)); c<=1000; c+=$i ))
	do 
   	prime[c]=0
	done
    fi
done

declare -i x=2;
declare -i y=0;

for i in ${nums[@]}; do

    if [[ ${prime[$i]} -ne 0 ]]; then
    #echo $x
        primes[y]=$((x))
        y=$((y+1))
    fi
    x=$((x+1))
done


cwd="$(pwd)"

cwd="${cwd}/input (1).txt"

while read -r line
do
line="$((${line//[ $'\001'-$'\037']}+7))"
  for i in ${primes[@]}; do
  if [[ $((line%i)) -eq 0 ]]; then
  echo -n $i" "	
  fi
  done
  echo
done < "$cwd"
