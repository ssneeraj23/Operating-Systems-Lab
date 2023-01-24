#!/usr/bin/bash
gcd(){
    x=$1
    y=$2
    if [ $y -gt $x ]  
    then
    t=$x
    x=$y
    y=$t
    fi
    r=$((x%y))
    while [ $r -ne 0 ]
    do 
    x=$y
    y=$r
    r=$((x%y))
    done
    g=$y
}
input=$1
lcm=1
g=1
while read -r v
do
  [ -z "$v" ] && continue
  v=`echo $v | rev`
  gcd $lcm $v
  lcm=$((((lcm)/g)*v)) 
done < "$input"
echo $lcm
