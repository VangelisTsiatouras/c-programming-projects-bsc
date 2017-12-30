#! /bin/sh
max=50
failures=0
for i in `seq 1 $max`
do
	printf "### TEST $i ###\n"

	printf "\nExecuting medium static\n"
	time ./ngrams -i ../datasets/medium_static/medium_static.init -q ../datasets/medium_static/medium_static.work > medium_static.out 2> medium_static_error.out

	
	printf "\nDifferences\n"
	diff medium_static.out ../datasets/medium_static/medium_static.result
	if [ $? -ne 0 ]
		then
		failures=$((failures+1))
	fi
	printf "\n"
done
printf "Failures: %d/%d\n" $((failures)) $((max))