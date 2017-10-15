#include <iostream>
#include "trie.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    Trie *trie = new Trie();
    trie->insert("asdf");
    trie->insert("aaaa");
    trie->insert("asdf");
    delete trie;
    cout << "exiting..." << endl;
    return 0;
}