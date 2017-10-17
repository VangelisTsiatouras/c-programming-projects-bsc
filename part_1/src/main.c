#include <stdlib.h>
#include <stdio.h>

#include "trie.h"

int main() {
    char ngram3[100] = "3three";
    char ngram4[100] = "4four";
    char ngram5[100] = "5five";
    char ngram6[100] = "6six";
    char ngram7[100] = "7seven";

    Trie *trie = trie_create();

    trie_insert(trie, ngram3);
    trie_insert(trie, ngram5);
    trie_insert(trie, ngram6);
    trie_insert(trie, ngram4);
    trie_insert(trie, ngram7);

    trie_node_delete(trie->root);
    trie_delete(trie);
    printf("exiting..!\n");
    return 0;
}
