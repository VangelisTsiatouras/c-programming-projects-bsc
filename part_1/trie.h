#ifndef TRIE_H
#define TRIE_H

#define STARTING_SIZE_CHILD_ARRAY 4

using namespace std;

class TrieNode {
private:
    string word;
    TrieNode **children;
    int occupiedPositions;
    int capacity;
    bool isFinal;
public:
    TrieNode();
    ~TrieNode();
    int insert(string newWord);
};

class Trie {
private:
    TrieNode *root;
public:
    Trie();
    ~Trie();
    int insert(string newWord);
};

#endif //TRIE_H
