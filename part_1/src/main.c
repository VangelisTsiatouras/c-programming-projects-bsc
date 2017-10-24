#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "trie.h"

int main() {

    srand (time(NULL));

//    char ngram1[100] = "1one";
//    char ngram2[100] = "2two";
//    char ngram3[100] = "3three";
//    char ngram4[100] = "4four";
//    char ngram5[100] = "5five";
//    char ngram6[100] = "6six";
//    char ngram7[100] = "7seven";
//    char ngram9[100] = "this is a";
//    char ngramLarge[100] = "one two three four five six";
//    char ngramLargeCOPY[100] = "one two three four five six";
//    char ngram8[100] = "this is a cat";
//    char queryngram[]= "this is cat and a one two three four five six";

    char assignment_ngram1[] = "this is a dog";
    char assignment_ngram2[] = "this is a cat";
    char assignment_ngram3[] = "the fox is bad";
    char assignment_ngram4[] = "the car is red";
    char assignment_ngram5[] = "this is a test";
    char assignment_ngram6[] = "this is a";
    char assignment_ngram7[] = "test";
    char assignment_ngram8[] = "fast car";

    char assignment_query1[] = "this is a cat and a dog";
    char assignment_query2[] = "this is not the test";
    char assignment_query3[] = "the car is red";
    char assignment_query4[] = "the car is red and fast";



    Trie *trie = trie_create();


//    for (int i = 0; i < 100; ++i) {
//        char *temp = malloc(20);
//        snprintf(temp, 20, "%.3d %.3d\n %.3d\n", rand() % 100, rand() % 100, rand() % 100);
//        trie_insert(trie, temp);
//        free(temp);
//    }

//    trie_query(trie, ngramLarge);
//
    trie_insert(trie, assignment_ngram1);
    trie_insert(trie, assignment_ngram2);
    trie_insert(trie, assignment_ngram3);
    trie_insert(trie, assignment_ngram4);
    trie_insert(trie, assignment_ngram5);
    trie_insert(trie, assignment_ngram6);

    trie_query(trie, assignment_query1);
    trie_query(trie, assignment_query2);

    trie_insert(trie, assignment_ngram7);

    char assignment_query2COPY[] = "this is not the test";
    trie_query(trie, assignment_query2COPY);

    trie_insert(trie, assignment_ngram8);

    trie_delete_ngram(trie, assignment_query3);

    trie_query(trie, assignment_query4);


    trie_destroy(trie);
    printf("exiting..!\n");
    return 0;
}
