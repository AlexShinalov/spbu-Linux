#!/bin/sh

cd ./repo
git checkout -b feature

cat >newFile.txt<<<"New text
var=1"

git add .
git commit -m "feat: add new file"

git checkout main
git merge feature
git branch -D feature

git log --oneline

