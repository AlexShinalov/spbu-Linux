#!/bin/bash
find . -name "*.jpg" | xargs mogrify -format png *.jpg
