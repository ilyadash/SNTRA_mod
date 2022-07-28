#!/bin/bash
files=`ls *xls`
#
homedir=`pwd`
for  f  in  ${files}
do
ssconvert $f "$f.csv" #use gnumeric command to convert files
find . -depth -name "*.xls.csv" -exec sh -c 'f="{}"; mv -- "$f" "${f%.xls.csv}.csv"' \; #remove .xls in .csv files names
#sed -i 's/\"/\ /g' "$f.csv"
#sed -i 's/\,/./g' "$f.csv
done
