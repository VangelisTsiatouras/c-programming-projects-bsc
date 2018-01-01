#! /bin/sh

max=1
smallFailures=0
mediumFailures=0
largeFailures=0

for i in `seq 1 $max`
do
	printf "##### TEST $i #####\n"
	printf "\n--- Executing small dynamic ---\n"
	time ./ngrams -i ../datasets/small_dynamic/small_dynamic.init -q ../datasets/small_dynamic/small_dynamic.work > small_dynamic.out

	printf "\n--- Executing medium dynamic ---\n"
	time ./ngrams -i ../datasets/medium_dynamic/medium_dynamic.init -q ../datasets/medium_dynamic/medium_dynamic.work > medium_dynamic.out

	printf "\n--- Executing large dynamic ---\n"
	time ./ngrams -i ../datasets/large_dynamic/large_dynamic.init -q ../datasets/large_dynamic/large_dynamic.work > large_dynamic.out

	
	printf "\n--- Differences ---"
	printf "\nSmall Dynamic\n"
	diff small_dynamic.out ../datasets/small_dynamic/small_dynamic.result
	if [ $? -ne 0 ]
		then
		smallFailures=$((smallFailures+1))
	else
		printf "No Differences!\n"
	fi
	printf "\nMedium Dynamic\n"
	diff medium_dynamic.out ../datasets/medium_dynamic/medium_dynamic.result
	if [ $? -ne 0 ]
		then
		mediumFailures=$((mediumFailures+1))
	else
		printf "No Differences!\n"
	fi
	printf "\nLarge Dynamic\n"
	diff large_dynamic.out ../datasets/large_dynamic/large_dynamic.result
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
