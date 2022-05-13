#!/bin/bash

for f in $(ls tdms/*.tdms)
do
    python tdms2root.py $f
done
