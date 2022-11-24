#!/bin/sh

mkdir repo
cd ./repo
git init

cat >helloWorld.txt<<<"Hello world"

git add .
git commit -m "Initial commit"
git log --oneline

