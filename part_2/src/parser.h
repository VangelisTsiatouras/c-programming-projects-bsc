#ifndef PARSER_H
#define PARSER_H

#include "trie.h"

int parser(Trie *trie, char *initFile, char *queryFile);
int dynamic_parser(Trie *trie, FILE *iFile, FILE *qFile);
int static_parser(Trie *trie, FILE *iFile, FILE *qFile);
int check_whitespace(char *line);

#endif //PARSER_H
