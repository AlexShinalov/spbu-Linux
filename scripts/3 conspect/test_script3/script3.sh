#!/bin/bash

func() {

cat &> $1 << EOF
$($2)
EOF
}

k=0
for i in $@
do
    k=$(($k+1))
    case "$k" in
	2) command=$i ;;
	*) command=$command" "$i ;;
    esac
done
file_out=$1
func "$file_out" "$command"
