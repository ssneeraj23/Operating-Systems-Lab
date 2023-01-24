#!/bin/bash

for (( i=1; i<=$#; i++)); do
    if [[ ${!i} =~ ^[0-9]{2}-[0-9]{2}-[0-9]{2}$ ]] 
    then 
        item=$((i+1))
        amount=$((i+2))
        name=$((i+3))
        echo "${!i},${!item},${!amount},${!name}" >> main.csv
        echo "Inserted ${!i},${!item},${!amount},${!name} in main.csv"
        sort -t ',' -k 1 -o main.csv main.csv
    fi
done

for ((i=1;i<=$#;i++)); do
    if [ "${!i}" == "-c" ]; 
    then
        j=$((i+1))
        category_total=$(awk -F, -v category="${!j}" '$2 == category { sum += $3 } END { print sum }' main.csv)
        echo "Amount spent in ${!j} is $category_total "
    elif [ "${!i}" == "-n" ]; 
    then
        j=$((i+1))
        name_total=$(awk -F, -v name="${!j}" '$4 == name { sum += $3 } END { print sum }' main.csv)
        echo "Amount spent by ${!j} is $name_total"
    elif [ "${!i}" == "-s" ]; 
    then
        j=$((i+1))
        if [ "${!j}" == "date" ]; 
        then
            sort -t ',' -k 1 main.csv -o main.csv 
        elif [ "${!j}" == "category" ]; 
        then
            sort -t ',' -k 2 main.csv -o main.csv
        elif [ "${!j}" == "amount" ]; 
        then
            sort -t ',' -k 3 main.csv -o main.csv
        else
            sort -t ',' -k 4 main.csv -o main.csv
        fi
    elif [ "${!i}" == "-h" ]; 
    then
        echo "Name: bash"
        echo "Usage: bash Assgn1_8_<32>.sh [-c category] [-n name] [-s column] record[date category amount name] [-h] "
    fi
done
