#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "trie.h"

int main() {

    srand (time(NULL));

    char ngram1[100] = "one";
    char ngram2[100] = "2two";
    char ngram3[100] = "3three";
    char ngram4[100] = "4four";
    char ngram5[100] = "5five";
    char ngram6[100] = "6six";
    char ngram7[100] = "7seven";
    char ngramLarge[100] = "one two three four five six";
    char ngramLargeCOPY[100] = "one two three four five six";

    char ngram8[100] = "this is a cat";
    char ngram9[100] = "this is a";

    char queryngram[]= "this is cat and a one two three four five six";

    Trie *trie = trie_create();

//    for (int i = 0; i < 100; ++i) {
//        char *temp = malloc(20);
//        snprintf(temp, 20, "%.3d %.3d\n %.3d\n", rand() % 100, rand() % 100, rand() % 100);
//        trie_insert(trie, temp);
//        free(temp);
//    }

//    trie_query(trie, ngramLarge);

    trie_insert(trie, ngramLarge);

    trie_query(trie, ngramLargeCOPY);

//    trie_dfs(trie->root);


//    trie_insert(trie, ngram5);
//    trie_insert(trie, ngram1);
//
//    trie_insert(trie, ngram3);
//
//    trie_insert(trie, ngram6);
//    trie_insert(trie, ngram4);
//    trie_insert(trie, ngram2);
//    trie_insert(trie, ngram7);

//    trie_node_print(trie->root);
//    trie_node_delete(trie->root);

    trie_delete(trie);
    printf("exiting..!\n");
    return 0;
}
