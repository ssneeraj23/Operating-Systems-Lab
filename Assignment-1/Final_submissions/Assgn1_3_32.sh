#!/bin/bash

if [[ $# -lt 3 ]]; then
	echo "Enter sufficient number of commands"
	exit -1
fi

json_dir=$1
csv_dir=$2

mkdir $2

cd $json_dir

for f in *.jsonl; do
cat $(basename $f) | sed -e ':a' -e 'N' -e '$!ba' -e 's/\n/,/g'  | sed 's/n/,/' | sed 's/^/[/'| sed 's/$/]/' > $(basename $f .jsonl).json
#echo "$(basename $f .jsonl).json"
done

cd ..

shift 2

ls

pwd

list=("$@")

declare -i m=0
 


for f in $json_dir/*.json; do
	cmd="jq -r '.[] | ["
	for s in $@;
	do
		echo $s
		cmd+=".$s, "
	done
	cmd=${cmd::-2}
	cmd+="] | @csv'"
	csv="$(pwd)/$csv_dir/$(basename $f .json).csv"
	filename="$(basename $file .json)"
	cmd+=" $json_dir/$(basename $f) > $csv"
	echo "$cmd"
	for k in "${list[@]}"; do
		echo "$k," > $csv.tmp
	done
	eval $cmd
	cat $csv >> $csv.tmp
	mv $csv.tmp $csv
	
done

for s in $json_dir/*.json; do
	rm "$json_dir/$(basename $s)"
done

for s in $json_dir/*.csv.tmp; do
	rm "$json_dir/$(basename $s)"
done

for s in $json_dir/*.csv; do
	rm "$json_dir/$(basename $s)"
done
