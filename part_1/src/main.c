#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "trie.h"

int main() {

    srand (time(NULL));

    char ngram1[100] = "1one";
    char ngram2[100] = "2two";
    char ngram3[100] = "3three";
    char ngram4[100] = "4four";
    char ngram5[100] = "5five";
    char ngram6[100] = "6six";
    char ngram7[100] = "7seven";

    Trie *trie = trie_create();

    for (int i = 0; i < 100; ++i) {
        char *temp = malloc(20);
        snprintf(temp, 20, "%.3d", rand() % 100);
        trie_insert(trie, temp);
    }



//    trie_insert(trie, ngram5);
//    trie_insert(trie, ngram1);
//
//    trie_insert(trie, ngram3);
//
//    trie_insert(trie, ngram6);
//    trie_insert(trie, ngram4);
//    trie_insert(trie, ngram2);
//    trie_insert(trie, ngram7);

    trie_node_print(trie->root);

    trie_node_delete(trie->root);
    trie_delete(trie);
    printf("exiting..!\n");
    return 0;
}
