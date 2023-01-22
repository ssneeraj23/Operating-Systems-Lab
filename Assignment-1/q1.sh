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
  lcm=$(((lcm*v)/g)) 
done < "$input"
echo $lcm


#using bc

#!/usr/bin/bash
gcd(){
    x=$1
    y=$2
    c=`echo "$y>$x" | bc`
    if [ $c -eq 1 ]
    then
    t=$x
    x=$y
    y=$t
    fi
    r=`echo "$x%$y" | bc`
    while [ $r -ne 0 ]
    do 
    x=$y
    y=$r
    r=`echo "$x%$y" | bc`
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
  lcm=`echo "($lcm*$v)/$g" | bc`
done < "$input"
echo $lcm
