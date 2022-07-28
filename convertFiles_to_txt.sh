#!/bin/bash
files=`ls *xls`
#
homedir=`pwd`
for  f  in  ${files}
do
ssconvert -O 'separator=" " format=raw' $f "$f.txt"
sed -i 's/\"/\ /g' "$f.txt"
sed -i 's/\,/./g' "$f.txt"
done
