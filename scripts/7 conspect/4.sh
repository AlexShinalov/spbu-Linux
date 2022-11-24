#!/bin/sh

rep='repo'
serv='serverRepo'
clone='repoClone'

function feat1 {
	git checkout -b feature/1

	cat >newFile.txt<<<"New text
var=2"
	
	git add .
	git commit -m "feat(newFile): change text 1"
	git push server feature/1
}

function feat2 {
	git checkout -b feature/2

	cat >newFile.txt<<<"New text
var=3"

	git add .
	git commit -m "feat(newFile): change text 2"
	git remote add server ../../$serv
	git push server feature/2
}

function merge {
	git pull server feature/1
	git checkout main
	git merge feature/1
	git merge feature/2
}

cd $rep
feat1

cd ../$clone/$serv
feat2

merge

