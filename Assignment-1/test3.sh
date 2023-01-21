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
        echo "Usage: bash Assgn1_8_<20CS30019>.sh [-c category] [-n name] [-s column] record[date category amount name] [-h] "
        echo "Description: 1.By default, the script accepts 4 arguments representing a new record (row) in csv and adds the record to main.csv."
        echo "2.The -c accepts category and prints the amount of money spent in that category."
        echo "3.The -n accepts name and prints the amount spent by that person."
        echo "4.The -s sorts the csv by column."
    fi
done

