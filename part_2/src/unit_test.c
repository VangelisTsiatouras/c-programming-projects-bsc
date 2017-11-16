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
#include "bloomfilter.h"
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

START_TEST (unitTestBloomFilter)
    {
        BloomFilter *bloomFilter = bloom_filter_create();
        ck_assert_int_eq(bloom_filter_check_insert(bloomFilter, "test1"), SUCCESS);
        ck_assert_int_eq(bloom_filter_check_insert(bloomFilter, "test2"), SUCCESS);
        ck_assert_int_eq(bloom_filter_check_insert(bloomFilter, "test1"), BLOOM_FILTER_FOUND);
        bloom_filter_set_to_zero(bloomFilter);
        ck_assert_int_eq(bloom_filter_check_insert(bloomFilter, "test1"), SUCCESS);
        bloom_filter_destroy(bloomFilter);
    }
END_TEST;

START_TEST (unitTestNgramCounter)
    {
        NgramCounter* nc = create_ngram_counter();
        ck_assert_int_eq(insert_ngram_counter(nc, "a"), 1);
        ck_assert_int_eq(insert_ngram_counter(nc, "b"), 1);
        ck_assert_int_eq(insert_ngram_counter(nc, "c"), 1);
        ck_assert_int_eq(insert_ngram_counter(nc, "d"), 1);
        ck_assert_int_eq(insert_ngram_counter(nc, "e"), 1);
        ck_assert_int_eq(insert_ngram_counter(nc, "e"), 0);
        ck_assert_int_eq(insert_ngram_counter(nc, "e"), 0);
        ck_assert_int_eq(insert_ngram_counter(nc, "a"), 0);
        print_ngram_counter(nc);
        NgramArray *ngramArray = copy_to_ngram_array(nc, nc->elements);
        print_ngram_array(ngramArray);
        sort_topk(ngramArray, 4);
        print_ngram_array(ngramArray);
        destroy_ngram_array(ngramArray);
        clear_ngram_counter(nc);
        ck_assert_int_eq(insert_ngram_counter(nc, "a"), 1);
        ngramArray = copy_to_ngram_array(nc, nc->elements);
        print_ngram_array(ngramArray);
        destroy_ngram_array(ngramArray);
        destroy_gram_counter(nc);
    }
END_TEST;

START_TEST(unitTestTrie)
    {
        char tNgram[100] = "test ngram";
        char tNgram2[100] = "test ngram";
        char tNgram3[100] = "test ngram";
        char tNgram4[100] = "test ngram2";
        // TRIENODE CHECK
        TrieNode *trieNode = NULL;
        // Create node with unallocated space
        ck_assert_int_eq(trie_node_create(trieNode), NOT_ALLOCATED_ERROR);

        trieNode = malloc(sizeof(TrieNode));
        // Create node with space
        ck_assert_int_eq(trie_node_create(trieNode), SUCCESS);
        free(trieNode);

        // TRIE CHECK
        // Create the trie
        Trie *trie = trie_create();
        ck_assert_ptr_ne(trie, NULL);
        // Insert an ngram
        ck_assert_int_eq(trie_insert(trie, tNgram), SUCCESS);
        // Check if binary search finds its first word
        SearchResults se = binary_search(trie->root->children, "test", trie->root->occupiedPositions);
        ck_assert_int_eq(se.found, 1);

        // These are already unit tested
        QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
        BloomFilter *bloomFilter = bloom_filter_create();
        NgramCounter *ngramCounter = create_ngram_counter();

        trie_query(trie, tNgram2, bloomFilter, queryResults, ngramCounter);
        copy_results_to_buffer_query_results(queryResults);
        flush_query_results(queryResults);
        // Check delete trie
        ck_assert_int_eq(trie_delete_ngram(trie, tNgram3), SUCCESS);
        ck_assert_int_eq(trie_delete_ngram(trie, tNgram4), DELETE_NOT_FOUND);
        ck_assert_int_eq(trie_destroy(trie), SUCCESS);

        destroy_query_results(queryResults);
        bloom_filter_destroy(bloomFilter);
        destroy_gram_counter(ngramCounter);
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

Suite *bloom_filter_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Bloom Filter");

    // Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, unitTestBloomFilter);
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
    Suite *querySuite, *trieSuite, *counterSuite, *bloomSuite;
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

    // Run test for bloomfilter
    bloomSuite = bloom_filter_suite();
    sr = srunner_create(bloomSuite);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    // Run test for trie
    trieSuite = trie_suite();
    sr = srunner_create(trieSuite);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
