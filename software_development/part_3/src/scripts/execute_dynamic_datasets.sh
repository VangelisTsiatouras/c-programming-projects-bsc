#!/bin/sh

alias time='/usr/bin/time'

max=5
smallFailures=0
mediumFailures=0
largeFailures=0

for i in `seq 1 $max`
do
	echo "##### TEST $i #####"
	echo "--- Executing small dynamic ---"
	time ../ngrams -i ../../datasets/small_dynamic/small_dynamic.init -q ../../datasets/small_dynamic/small_dynamic.work > small_dynamic.out
	echo
	echo "--- Executing medium dynamic ---"
	time ../ngrams -i ../../datasets/medium_dynamic/medium_dynamic.init -q ../../datasets/medium_dynamic/medium_dynamic.work > medium_dynamic.out
	echo
	echo "--- Executing large dynamic ---"
	time ../ngrams -i ../../datasets/large_dynamic/large_dynamic.init -q ../../datasets/large_dynamic/large_dynamic.work > large_dynamic.out

	echo
	echo "--- Differences ---"
	echo "Small Dynamic"
	diff small_dynamic.out ../../datasets/small_dynamic/small_dynamic.result
	if [ $? -ne 0 ]
		then
		smallFailures=$((smallFailures+1))
	else
		echo "No Differences!"
	fi
	echo
	echo "Medium Dynamic"
	diff medium_dynamic.out ../../datasets/medium_dynamic/medium_dynamic.result
	if [ $? -ne 0 ]
		then
		mediumFailures=$((mediumFailures+1))
	else
		echo "No Differences!"
	fi
	echo
	echo "Large Dynamic"
	diff large_dynamic.out ../../datasets/large_dynamic/large_dynamic.result
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
