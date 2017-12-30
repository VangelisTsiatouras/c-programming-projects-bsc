#! /bin/sh

max=20
smallFailures=0
mediumFailures=0
largeFailures=0

for i in `seq 1 $max`
do
	printf "##### TEST $i #####\n"
	printf "\n--- Executing small static ---\n"
	time ./ngrams -i ../datasets/small_static/small_static.init -q ../datasets/small_static/small_static.work > small_static.out

	printf "\n--- Executing medium static ---\n"
	time ./ngrams -i ../datasets/medium_static/medium_static.init -q ../datasets/medium_static/medium_static.work > medium_static.out

	printf "\n--- Executing large static ---\n"
	time ./ngrams -i ../datasets/large_static/large_static.init -q ../datasets/large_static/large_static.work > large_static.out

	
	printf "\n--- Differences ---"
	printf "\nSmall Static\n"
	diff small_static.out ../datasets/small_static/small_static.result
	if [ $? -ne 0 ]
		then
		smallFailures=$((smallFailures+1))
	else
		printf "No Differences!\n"
	fi
	printf "\nMedium Static\n"
	diff medium_static.out ../datasets/medium_static/medium_static.result
	if [ $? -ne 0 ]
		then
		mediumFailures=$((mediumFailures+1))
	else
		printf "No Differences!\n"
	fi
	printf "\nLarge Static\n"
	diff large_static.out ../datasets/large_static/large_static.result
	if [ $? -ne 0 ]
		then
		largeFailures=$((largeFailures+1))
	else
		printf "No Differences!\n"
	fi
	printf "\n"
done
printf "Failures in small dataset: %d/%d\n" $((smallFailures)) $((max))
printf "Failures in medium dataset: %d/%d\n" $((mediumFailures)) $((max))
printf "Failures in large dataset: %d/%d\n" $((largeFailures)) $((max))
