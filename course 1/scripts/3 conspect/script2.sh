#!/bin/bash
while getopts x:y: args; do
case $args in
	 x)shir=$OPTARG;;
	 y)dl=$OPTARG;;
esac
done
curl wttr.in/$shir,$dl	

