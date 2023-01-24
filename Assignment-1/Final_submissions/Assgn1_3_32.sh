#!/usr/bin/bash
source=$1
destination=$2
shift 2
req_atrbts=("$@")
if [ ! -d $destination ]; then mkdir $destination; fi
for fname in $source/*.jsonl; do
    jq_req_atrbts=""
    csv_fname="$destination/$(basename $fname .jsonl).csv"
    for temp in "${req_atrbts[@]}"; do jq_req_atrbts+=" .$temp,"; done
    jq_req_atrbts="${jq_req_atrbts%?}"
    jq -r -c "select([$jq_req_atrbts] | all(.; . != null)) | [$jq_req_atrbts] | @csv" $fname >$csv_fname
    echo ${req_atrbts[*]} | tr ' ' ',' >$csv_fname.tmp
    cat $csv_fname >>$csv_fname.tmp
    mv $csv_fname.tmp $csv_fname
done
