#!/bin/bash
if [ ! -f main.csv ]; then   
     echo "date,category,amount,name" > main.csv
fi
new_row='date,category,amount,name'
cat main.csv
if head -n 1 main.csv | grep -q $new_row; then
    tail -n +2 main.csv > main.csv
    echo "Deleted first line"
    cat main.csv
fi
for (( i=1; i<=$#; i++)); do
    if [[ ${!i} =~ ^[0-9]{2}-[0-9]{2}-[0-9]{2}$ ]] 
    then 
        item=$((i+1))
        amount=$((i+2))
        name=$((i+3))
        echo "${!i},${!item},${!amount},${!name}" >> main.csv

        echo "Inserted ${!i},${!item},${!amount},${!name} in main.csv"
        sort -t ',' -k 1 -o main.csv main.csv
        cat main.csv
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
    fi
done

sed -i '1i\'$new_row'' main.csv 

