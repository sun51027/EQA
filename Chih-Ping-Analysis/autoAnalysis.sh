#!/bin/bash
./autoClassifying.sh
#inputEnd=$(date +%Y%m%d000000)
inputEnd=20210422000000
changeTxtfile='document/repeatProduceDoc.txt'
sed -i "/EndTime:/d" $changeTxtfile
sed -i "1iEndTime: "$inputEnd $changeTxtfile
./go -up -TI -if $changeTxtfile -of test3
./go -fit -if test3 -of fitTest
