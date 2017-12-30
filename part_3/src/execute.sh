#! /bin/sh
max=10
for i in `seq 1 $max`
do
	printf "### TEST $i ###\n"
	printf "\nExecuting large static\n"
	time ./ngrams -i ../datasets/large_static/large_static.init -q ../datasets/large_static/large_static.work > large_static.out

	printf "Executing small static\n"
	time ./ngrams -i ../datasets/small_static/small_static.init -q ../datasets/small_static/small_static.work > small_static.out

	printf "\nExecuting medium static\n"
	time ./ngrams -i ../datasets/medium_static/medium_static.init -q ../datasets/medium_static/medium_static.work > medium_static.out 2> medium_static_error.out

	
	printf "\nDifferences\n"
	printf "\nSmall Static\n"
	diff small_static.out ../datasets/small_static/small_static.result
	printf "\nMedium Static\n"
	diff medium_static.out ../datasets/medium_static/medium_static.result
	printf "\nLarge Static\n"
	diff large_static.out ../datasets/large_static/large_static.result
	printf "\n"
done
