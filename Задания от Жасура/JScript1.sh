#!/bin/bash
set -e
num=$1
maxs=$num
while [ $num -ne 1 ];
do
	echo "$num"
	q=$(($num%2))
	if [ $q -eq 1 ]
	then
		num=$((3*$num+1))
		if [ $num -gt $maxs ]
		then
			maxs=$num
		fi
	else
		num=$(($num/2))
	fi
done
echo "max=$maxs"


