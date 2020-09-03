#!/bin/bash

if [[ "$1" == "clean" ]]; then
	for s in *.adv *.obj tmp.xmi *.drv *.exe; do
		echo "$s"
		if [[ "$s" != "msmake.exe" && -f "$s" ]]; then
			rm -v $s
		fi
	done
	exit 0;
fi

dosbox -conf dosbox/dosbox.conf -c @D:\build.bat

for SRC in *
do
    DST=`dirname "${SRC}"`/`basename "${SRC}" | tr '[A-Z]' '[a-z]'`
    if [[ "${SRC}" != "${DST}" && "${SRC}" != "Makefile" ]]
    then
        [ ! -e "${DST}" ] && mv -T "${SRC}" "${DST}" || echo "${SRC} was not renamed"
    fi
done

