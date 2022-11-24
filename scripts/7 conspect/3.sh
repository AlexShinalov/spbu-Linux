#!/bin/sh

repo="repo"
serv="serverRepo"
clone="repoClone"

mkdir $serv
cd $serv

git init --bare

cd ../$repo

git remote add server ../$serv
git push server main

cd ../
mkdir $clone
cd $clone

git clone ../$serv

cd ./$serv

git log --oneline

