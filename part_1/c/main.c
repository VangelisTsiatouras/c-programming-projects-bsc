#include <stdlib.h>
#include <stdio.h>

#include "trie.h"

int main() {
    printf("Hello, World!\n");
    Trie *trie = trie_create();
    trie_node_delete(trie->root);
    trie_delete(trie);
    printf("exiting..!\n");
    return 0;
}
