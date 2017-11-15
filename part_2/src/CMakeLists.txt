cmake_minimum_required(VERSION 3.8)
project(ngrams)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c parser.c parser.h trie.h trie.c queryresults.c queryresults.h defs.h bloomfilter.c bloomfilter.h hashfunctions.c hashfunctions.h ngramcounter.c ngramcounter.h)
add_executable(ngrams ${SOURCE_FILES})