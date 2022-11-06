#!/bin/bash
type=$(file --mime-type $1)
type=${type##$1: }
case $type in
	"image/jpeg") gimp $1 ;;
	"application/vnd.openxmlformats-officedocument.wordprocessingml.document") libreoffice $1 ;;
	"video/ogg") mpv $1 ;;
	*) echo "Неизвестный формат файла" ;;
esac
