cmake_minimum_required(VERSION 3.8)
project(trie)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c parser.c parser.h trie.h trie.c queryresults.c queryresults.h defs.h)
add_executable(trie ${SOURCE_FILES})