cmake_minimum_required(VERSION 3.8)
project(ngrams)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c parser/parser.c parser/parser.h trie/trie.h trie/trie.c queryresults/queryresults.c queryresults/queryresults.h definitions.h bloomfilter/bloomfilter.c bloomfilter/bloomfilter.h hashfunctions/hashfunctions.c hashfunctions/hashfunctions.h ngramcounter/ngramcounter.c ngramcounter/ngramcounter.h linearhash/linearhash.c linearhash/linearhash.h threadpool/threadpool.c threadpool/threadpool.h)
add_executable(ngrams ${SOURCE_FILES})

target_link_libraries(ngrams m)