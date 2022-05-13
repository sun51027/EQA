#!/bin/bash
for f in $(find data -maxdepth 1)
do

    if test -f $f
    then
	filename=$(basename $f)
	pathname=$(dirname $f)
	newdir=${filename:0:8}
	goaldir="$pathname/$newdir"

	if !(test -e $goaldir)
	then
	    mkdir $goaldir
	fi
	mv $f $goaldir
    fi
done
