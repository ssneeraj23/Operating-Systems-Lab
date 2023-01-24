#!/bin/bash
mkdir $2
cd $2
touch {a..z}.txt
cd $1
for FILE in *
do
 while read line
 do
   t=${line:0:1}
   t=${t,,}
   echo $line | sort -m $2/$t.txt - > $2/$t.txt.new && mv $2/$t.txt{.new,}
 done < $FILE
done
