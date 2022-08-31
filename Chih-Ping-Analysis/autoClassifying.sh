#!/bin/bash
for f in $(find /wk_cms2/wuhsinyeh/public/EQ_detector/radonData/2022/ -maxdepth 1 | grep root)
do

    if test -f $f
    then
	filename=$(basename $f)
	pathname=$(dirname $f)
	newdir=${filename:0:8}
	goaldir="/wk_cms2/shihlin0314/CMSSW_8_0_29/src/Earthquake/EQA/Chih-Ping-Analysis/$newdir"

	if !(test -e $goaldir)
	then
	    mkdir $goaldir
	fi
	cp $f $goaldir
    fi
done
