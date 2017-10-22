cmake_minimum_required(VERSION 3.8)
project(tries)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c trie.h trie.c)
add_executable(tries ${SOURCE_FILES})