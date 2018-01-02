#!/bin/sh

alias time='/usr/bin/time'

max=20
smallFailures=0
mediumFailures=0
largeFailures=0

for i in `seq 1 $max`
do
	echo "##### TEST $i #####"
	echo "--- Executing small static ---"
	time ./ngrams -i ../datasets/small_static/small_static.init -q ../datasets/small_static/small_static.work > small_static.out
	echo
	echo "--- Executing medium static ---"
	time ./ngrams -i ../datasets/medium_static/medium_static.init -q ../datasets/medium_static/medium_static.work > medium_static.out
	echo
	echo "--- Executing large static ---"
	time ./ngrams -i ../datasets/large_static/large_static.init -q ../datasets/large_static/large_static.work > large_static.out
	
	echo
	echo "--- Differences ---"
	echo "Small Static"
	diff small_static.out ../datasets/small_static/small_static.result
	if [ $? -ne 0 ]
		then
		smallFailures=$((smallFailures+1))
	else
		echo "No Differences!"
	fi
	echo
	echo "Medium Static"
	diff medium_static.out ../datasets/medium_static/medium_static.result
	if [ $? -ne 0 ]
		then
		mediumFailures=$((mediumFailures+1))
	else
		echo "No Differences!"
	fi
	echo
	echo "Large Static"
	diff large_static.out ../datasets/large_static/large_static.result
	if [ $? -ne 0 ]
		then
		largeFailures=$((largeFailures+1))
	else
		echo "No Differences!"
	fi
	echo
done
echo "Failures in small dataset: $((smallFailures))/$((max))"
echo "Failures in medium dataset: $((mediumFailures))/$((max))"
echo "Failures in large dataset: $((largeFailures))/$((max))"
