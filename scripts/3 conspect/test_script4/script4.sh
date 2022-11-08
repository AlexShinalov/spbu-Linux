#!/bin/bash
all="$(find $1 -depth)"

for name in ${all}; do
new_name="$(dirname "${name}")/$(basename "${name}" | tr '[a-z]' '[A-Z]')"
[ ! -e "${new_name}" ] && mv -T "${name}" "${new_name}"; 
done
exit 0
