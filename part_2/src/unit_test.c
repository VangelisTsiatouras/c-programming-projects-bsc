#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Use "Check" framework
#include <check.h>
#include <stdint.h>

#include "trie.h"
#include "queryresults.h"
#include "ngramcounter.h"
#include "defs.h"

START_TEST (unitTestQueryResults)
    {
        QueryResults *queryResults;
        queryResults = create_query_results(3, 20);
        ck_assert_int_eq(add_line_query_results(queryResults, "Test sentence"), SUCCESS);
        ck_assert_int_eq(add_line_query_results(queryResults, "Second sentence"), SUCCESS);
        // Add a big sentence
        ck_assert_int_eq(
                add_line_query_results(queryResults, "This is a big line. This is a big line. This is a big line"),
                SUCCESS);
        copy_results_to_buffer_query_results(queryResults);
        // Add a sentence when you dont have space
        ck_assert_int_eq(add_line_query_results(queryResults, "Last sentence"), SUCCESS);
        // Print
        copy_results_to_buffer_query_results(queryResults);
        flush_query_results(queryResults);
        // Destroy
        destroy_query_results(queryResults);
    }
END_TEST;

START_TEST (unitTestNgramCounter)
    {
        NgramCounter* nc = create_ngram_counter();
        ck_assert_int_eq(insert_ngram_counter(nc, "a"), 1);
        insert_ngram_counter(nc, "b");
        insert_ngram_counter(nc, "c");
        insert_ngram_counter(nc, "d");
        insert_ngram_counter(nc, "e");
        insert_ngram_counter(nc, "e");
        insert_ngram_counter(nc, "e");
        print_ngram_counter(nc);
//        clear_ngram_counter(nc);
//        destroy_gram_counter(nc);
    }
END_TEST;

START_TEST(unitTestTrie)
    {
//        char tNgram[100] = "test ngram";
//        char tNgram2[100] = "test ngram";
//        char tNgram3[100] = "test ngram";
//        char tNgram4[100] = "test ngram2";
//        // TRIENODE CHECK
//        TrieNode *trieNode = NULL;
//        // Create node with unallocated space
//        ck_assert_int_eq(trie_node_create(trieNode), NOT_ALLOCATED_ERROR);
//
//        trieNode = malloc(sizeof(TrieNode));
//        // Create node with space
//        ck_assert_int_eq(trie_node_create(trieNode), SUCCESS);
//        free(trieNode);
//
//        // TRIE CHECK
//        // Create the trie
//        Trie *trie = trie_create();
//        ck_assert_ptr_ne(trie, NULL);
//        // Insert an ngram
//        ck_assert_int_eq(trie_insert(trie, tNgram), SUCCESS);
//        // Check if binary search finds its first word
//        SearchResults se = binary_search(trie->root->children, "test", trie->root->occupiedPositions);
//        ck_assert_int_eq(se.found, 1);
//
//        // QueryResults is already unit tested
//        QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
//        trie_query(trie, tNgram2, queryResults);
//        copy_results_to_buffer_query_results(queryResults);
//        flush_query_results(queryResults);
//        // Check delete trie
//        ck_assert_int_eq(trie_delete_ngram(trie, tNgram3), SUCCESS);
//        ck_assert_int_eq(trie_delete_ngram(trie, tNgram4), DELETE_NOT_FOUND);
//        ck_assert_int_eq(trie_destroy(trie), SUCCESS);
    }
END_TEST;

Suite *query_results_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("QueryResults");

    // Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, unitTestQueryResults);
    suite_add_tcase(s, tc_core);

    return s;
}

Suite *ngram_counter_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("ngram counter");

    // Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, unitTestNgramCounter);
    suite_add_tcase(s, tc_core);

    return s;
}

Suite *trie_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Trie");

    // Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, unitTestTrie);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void) {
    int number_failed;
    Suite *querySuite, *trieSuite, *counterSuite;
    SRunner *sr;

    // Run test for query results
    querySuite = query_results_suite();
    sr = srunner_create(querySuite);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    // Run test for ngramcounter
    counterSuite = ngram_counter_suite();
    sr = srunner_create(counterSuite);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

//    // Run test for trie
//    trieSuite = trie_suite();
//    sr = srunner_create(trieSuite);
//    srunner_run_all(sr, CK_NORMAL);
//    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
