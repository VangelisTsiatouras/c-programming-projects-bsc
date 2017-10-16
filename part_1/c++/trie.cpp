#include <iostream>
#include "trie.h"

TrieNode::TrieNode() {
    std::cout << "TrieNode constructor" << std::endl;
    occupiedPositions = 0;
    capacity = STARTING_SIZE_CHILD_ARRAY;
    children = (TrieNode**) malloc(capacity * sizeof(TrieNode));
    if (!children) {
        cout << "Could not allocate memory!" << endl;
        exit(-1);
    }
    //Mark all children as empty
//    for (int i = 0; i < capacity; i++) {
//        children[i]->word = ("");
//    }
}

TrieNode::~TrieNode() {
    std::cout << "TrieNode destructor" << std::endl;
}

int TrieNode::insert(string newWord) {
    // if(occupiedPositions < capacity) {
    //     for (int i = 0; i < capacity; i++) {
    //         if(children[i]->word == ""){
    //             cout << "aaaaa" << endl;
    //             int result = newWord.compare(children[i]->word);
    //             if (result == 0) {
    //                 cout << "exists already" << endl;
    //                 return 0;
    //             }
    //         } else {
    //             children[i]->word = newWord;
    //             occupiedPositions++;
    //             break;
    //         }
    //     }
    // } else {
    //     return -1;
    // }
}

////////////////////////////////////////////////////////////

Trie::Trie() {
    std::cout << "Trie constructor" << std::endl;
    root = new TrieNode();
}

Trie::~Trie() {
    std::cout << "Trie destructor" << std::endl;
}

int Trie::insert(string newWord) {
    return root->insert(newWord);
}
