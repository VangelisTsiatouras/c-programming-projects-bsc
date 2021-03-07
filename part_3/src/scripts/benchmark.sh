#!/bin/sh

alias time='/usr/bin/time'

max=5
sum=0
average=0

echo "Testing small static dataset"
for i in `seq 1 $max`
do
	ts=$(date +%s%N) 
	../ngrams -i ../../datasets/small_static/small_static.init -q ../../datasets/small_static/small_static.work > small_static.out
	tt=$((($(date +%s%N) - $ts)/1000000))
	sum=$(($sum+$tt))
	echo "$tt msec"
done
average=$(($sum/$max))
echo "Average $average msec"

sum=0
average=0
echo
echo "Testing small dynamic dataset"
for i in `seq 1 $max`
do
	ts=$(date +%s%N) 
	../ngrams -i ../../datasets/small_dynamic/small_dynamic.init -q ../../datasets/small_dynamic/small_dynamic.work > small_dynamic.out
	tt=$((($(date +%s%N) - $ts)/1000000))
	sum=$(($sum+$tt))
	echo "$tt msec"
done
average=$(($sum/$max))
echo "Average $average msec"

sum=0
average=0
echo
echo "Testing medium static dataset"
for i in `seq 1 $max`
do
	ts=$(date +%s%N) 
	../ngrams -i ../../datasets/medium_static/medium_static.init -q ../../datasets/medium_static/medium_static.work > medium_static.out
	tt=$((($(date +%s%N) - $ts)/1000000))
	sum=$(($sum+$tt))
	echo "$tt msec"
done
average=$(($sum/$max))
echo "Average $average msec"

sum=0
average=0
echo
echo "Testing medium dynamic dataset"
for i in `seq 1 $max`
do
	ts=$(date +%s%N) 
	../ngrams -i ../../datasets/medium_dynamic/medium_dynamic.init -q ../../datasets/medium_dynamic/medium_dynamic.work > medium_dynamic.out
	tt=$((($(date +%s%N) - $ts)/1000000))
	sum=$(($sum+$tt))
	echo "$tt msec"
done
average=$(($sum/$max))
echo "Average $average msec"

sum=0
average=0
echo
echo "Testing large static dataset"
for i in `seq 1 $max`
do
	ts=$(date +%s%N) 
	../ngrams -i ../../datasets/large_static/large_static.init -q ../../datasets/large_static/large_static.work > large_static.out
	tt=$((($(date +%s%N) - $ts)/1000000))
	sum=$(($sum+$tt))
	echo "$tt msec"
done
average=$(($sum/$max))
echo "Average $average msec"

sum=0
average=0
echo
echo "Testing large dynamic dataset"
for i in `seq 1 $max`
do
	ts=$(date +%s%N) 
	../ngrams -i ../../datasets/large_dynamic/large_dynamic.init -q ../../datasets/large_dynamic/large_dynamic.work > large_dynamic.out
	tt=$((($(date +%s%N) - $ts)/1000000))
	sum=$(($sum+$tt))
	echo "$tt msec"
done
average=$(($sum/$max))
echo "Average $average msec"
