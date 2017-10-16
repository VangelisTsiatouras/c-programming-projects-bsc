#include <stdlib.h>
#include <stdio.h>

#include "trie.h"

int main() {
    printf("Hello, World!\n");
    Trie *trie = trie_create();
    trie_delete(trie);
    printf("exiting..!\n");
    return 0;
}
