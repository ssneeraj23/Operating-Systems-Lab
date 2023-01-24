#!/usr/bin/bash
unique_names=($(awk '{print $1}' majors.txt | sort -u))
unique_majors=($(awk '{print $2}' majors.txt | sort -u))
single_major_count=0
multiple_major_students=""
for major in "${unique_majors[@]}"
do
    echo "$major $(grep -c "$major" majors.txt)"
done | sort -k2,2nr -k1,1
for name in "${unique_names[@]}"
do
    if [ $(grep -c "$name" majors.txt) -gt 1 ]; then
        multiple_major_students="$multiple_major_students $name"
    else
        single_major_count=$((single_major_count+1))
    fi
done
echo " "
echo "$multiple_major_students"
echo $single_major_count
