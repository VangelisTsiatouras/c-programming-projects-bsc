# Software Development for Information Systems

## About

This project is based on [ACM SIGMOD Programming Contest 2017](http://sigmod17contest.athenarc.gr/) and it was assignment for the course "Software Development for Information Systems". The task of this project was to implement a [trie](https://en.wikipedia.org/wiki/Trie) that stores [N-grams](https://en.wikipedia.org/wiki/N-gram). The next crucial task was to handle queries and return results, as quickly as possible. The project was split into 3 parts and is written in C. The first part was about implementing the base data structure of the trie, the second was mostly optimizations over the first part and the last part was to add multithread support. The program at the beginning of the execution, initializes the trie struct from a dataset that contains multiple N-grams. After that, the program reads from another dataset the queries that will handle. These queries are documents (texts with >1000 words) and these datasets vary at sizes (small dataset: ~16MB, medium dataset: ~300MB, large dataset: ~220MB ). Also the program supports the following operations:

* 'Q'/query: This operation needs to be answered with the N-grams that have been found in the document. The program provides a line for each document. The line contains all the extracted N-grams separated by '|'. If there are no extracted N-grams, the program should answer with -1.
* 'A'/add: This operation adds a new one N-gram to the trie. If the specified N-gram already exists, the trie should remain unchanged.
* 'D'/delete: This operation removes a N-gram that is stored inside the trie. If the specified N-gram does not exist, the trie should remain unchanged.
* 'F [number with top N-grams]'/end of burst: This operation defines where a set of operations ends(burst). Also the program flushes out all the output from the operations and also displays the top K N-grams that printed out the most times at the current burst.

The implementation phases are the following: 

## Part 1

In this phase we have implemented a data structure based on the following:

```c
typedef struct TrieNode {
    char word[WORD_SIZE];
    char *largeWord;
    struct TrieNode *children;
    int occupiedPositions;
    int capacity;
    char isFinal;
} TrieNode;

typedef struct Trie {
    TrieNode *root;
} Trie;
```

![Trie](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/trie.png)

The algorithms that used for the insertions, deletions and the queries are [Depth-First Search](https://en.wikipedia.org/wiki/Depth-first_search) and [Binary Search](https://en.wikipedia.org/wiki/Binary_search_algorithm). The main concept of the implementation was the below:

* In each N-gram given, start from the root of the trie.
* If the first word is stored in the root, continue with the next word of the N-gram to the next level of the trie (here we apply binary search at childern array to find the next node fast).
* Pick the next word of the input N-gram.
* Repeat step 2 until no words of the N-gram are found inside the trie.

With this algorithm the program prints out correct results but it was pretty slow... The execution time in the small dataset was approximately 1 second, 41 seconds on medium dataset and 147 seconds on large dataset. In the next part we optimized the whole data structure significantly.

## Part 2

One important problem that causes massive slowdown at the execution is that if a Query contains duplicate N-grams, the  algorithm form Part 1 searches them again and again. To avoid this we have added a [Bloom Filter](https://en.wikipedia.org/wiki/Bloom_filter), to filter these duplicate N-grams that already have been searched. With that way, the search algorithm executes only on N-grams that are unique on each Query. For the implementation of the Bloom Filter we used 3 [murmur3](https://en.wikipedia.org/wiki/MurmurHash) hash functions and a large bit array. According to our calculations the false positive probability is ~0.001% and with the current datasets the filter seems to work well.

![Bloom Filter False](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/bloom_filter_false.png)
![Bloom Filter Positive](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/bloom_filter_positive.png)

The next optimization was to apply [Linear Hashing](https://en.wikipedia.org/wiki/Linear_hashing) at the root node of the trie. The reason of this modification was that the children node array of the root of the trie, expands very fast and as a result searching inside this array becomes slower. With the linear hashing this problem is solved and the search algorithm applies inside the buckets of the Hash Table which are significantly smaller. In that way the program gains a satisfying performance boost.

An extra feature we added in this part was to find the top k N-ngrams that printed out the most times in one burst of commands. To implement this feature we used 2 basic and fast sorting algorithms which are [Quickselect](https://en.wikipedia.org/wiki/Quickselect) and [Quicksort](https://en.wikipedia.org/wiki/Quicksort). With these algorithms combined the results are calculated very fast. Also an other feature we added was the static trie mode. In this mode the trie after the init compresses the nodes that have only one child and after that the trie cannot expand or delete nodes (the work file doesn't contain 'A' and 'D' operations). The biggest benefit of the static trie is that the proram now consumes a lot less memory.

![Static Trie](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/static_trie.png)

In conclusion, with all above combined the program execution times dropped and more precisely in small dataset was 0.9 sec, in medium was 28 secs and in large dataset was 32 seconds!

## Part 3

This part is the final part of this project and the most impacting in terms of performance gain. The main optimization we added in this part was the [multi thread](https://en.wikipedia.org/wiki/POSIX_Threads) support. To implement this we created a [generic thread pool](https://en.wikipedia.org/wiki/Thread_pool) which is responsible for the job scheduling. Algorithmically, to implement parrallelism, all the queries('Q') are executing in parallel and the appends('A') and deletions('D') are executed at the end of each burst. To avoid inconsistency at the data that are stored in the trie due to the parallelism, we have added versioning on the trie structure and each version is affected by the additions('A') and the deletions('D') operations. In that way the queries('Q') choose the correct data based on this condition: `ngram.Append_version ≤ query.version και ngram.Delete_version > query.version`.

After all these optimizations we have achieved impressive execution times. In small dataset the execution time was ~0.1 sec, at medium ~3.5 seconds and at large dataset was ~9 seconds.

![Execution times](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/execution_times.png)
![Med Dyn memory](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/dyn_mem_med.png)
![Med Static memory](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/dyn_mem_med.png)
![Large Dyn memory](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/dyn_mem_large.png)
![Large Static memory](https://github.com/VangelisTsiatouras/software-development-di/blob/master/readme_assets/static_mem_large.png)
