#!/bin/sh

filesdir=$1
searchstr=$2
num_line=0
num_file=0
old_line=""

if [ $# -lt 2 ]
then
    echo "Wrong format."
    exit 1
fi

if [ ! -d "$filesdir" ]
then
    echo "Directory doesn't exist."
    exit 1
fi

grep $searchstr -r $filesdir > /tmp/find_result.txt

input="/tmp/find_result.txt"

while read -r line
do
    num_line=$(($num_line + 1))
    tmp=$(echo "$line" | awk -F ':' '{print $1}')
    if [ "$tmp" != "$old_line" ]
    then
        num_file=$(($num_file + 1))
        old_line=$tmp
    fi
done < "$input"

if [ $num_line -eq 0 ]
then
    exit 1
fi

rm -rf /tmp/find_result.txt
echo "The number of files are $num_file and the number of matching lines are $num_line"
