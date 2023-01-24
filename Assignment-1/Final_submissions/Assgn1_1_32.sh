#!/usr/bin/bash
input=$1
lcm=1
g=1
while read -r v
do
  [ -z "$v" ] && continue
    v=`echo $v | rev`
    x=$lcm;y=$v
    if [ $y -gt $x ];then t=$x;x=$y;y=$t;fi
    r=$((x%y))
    while [ $r -ne 0 ];do x=$y;y=$r;r=$((x%y));done
    g=$y;lcm=$((((lcm)/g)*v)) 
done < "$input"
echo $lcm
