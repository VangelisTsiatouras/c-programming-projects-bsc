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
#include "linearhash.h"
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
        BloomFilter *bloomFilter = create_bloom_filter();
        ck_assert_int_eq(check_insert_bloom_filter(bloomFilter, "test1"), SUCCESS);
        ck_assert_int_eq(check_insert_bloom_filter(bloomFilter, "test2"), SUCCESS);
        ck_assert_int_eq(check_insert_bloom_filter(bloomFilter, "test1"), BLOOM_FILTER_FOUND);
        set_to_zero_bloom_filter(bloomFilter);
        ck_assert_int_eq(check_insert_bloom_filter(bloomFilter, "test1"), SUCCESS);
        set_to_zero_bloom_filter(bloomFilter);
        destroy_bloom_filter(bloomFilter);
    }
END_TEST;

START_TEST (unitTestNgramCounter)
    {
        // Na to tsekarei o Nikos an einai swsta ta parakatw
        // gt sto copy_to_ngram_array eixe alla3ei to def
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
        NgramArray *ngramArray = copy_to_ngram_array(nc);
        print_ngram_array(ngramArray);
        sort_topk(ngramArray, 4);
        print_ngram_array(ngramArray);
        destroy_ngram_array(ngramArray);
        clear_ngram_counter(nc);
        ck_assert_int_eq(insert_ngram_counter(nc, "a"), 1);
        ngramArray = copy_to_ngram_array(nc);
        print_ngram_array(ngramArray);
        destroy_ngram_array(ngramArray);
        destroy_gram_counter(nc);
    }
END_TEST;

START_TEST(unitLinearHash)
    {
        // Test Bucket
        LHBucket *lhBucket = create_LHBucket();
        ck_assert_int_eq(insert_word_LHBucket(lhBucket, "d"), 0);
        ck_assert_int_eq(insert_word_LHBucket(lhBucket, "c"), 0);
        ck_assert_int_eq(insert_word_LHBucket(lhBucket, "a"), 0);
        ck_assert_int_eq(insert_word_LHBucket(lhBucket, "f"), 0);
        ck_assert_int_eq(insert_word_LHBucket(lhBucket, "b"), 1);

        TrieNode t;
        create_trie_node(&t);
        store_word_trie_node(&t, "test");
        ck_assert_int_eq(insert_trie_node_LHBucket(lhBucket, &t), 0);
        print_LHBucket(lhBucket);

        delete_word_LHBucket(lhBucket,"f");
        delete_word_LHBucket(lhBucket,"b");
        delete_word_LHBucket(lhBucket,"test");
        delete_word_LHBucket(lhBucket,"asdf");
        print_LHBucket(lhBucket);

        destroy_LHBucket(lhBucket);

        // Test hashtable
        LinearHash *linearHash = create_LinearHash();

//        char string[20] = "aaaa";
//        for (int i = 0; i < 500000; i++) {
//            char c = rand() % 26 + 'a';
//            string[0] = c;
//            c = rand() % 26 + 'a';
//            string[1] = c;
//            c = rand() % 26 + 'a';
//            string[2] = c;
//            c = rand() % 26 + 'a';
//            string[3] = c;
//            insert_LinearHash(linearHash, string);
//        }

        insert_LinearHash(linearHash, "test1");
        insert_LinearHash(linearHash, "test2");
        insert_LinearHash(linearHash, "test3");

        print_LinearHash(linearHash);
        TrieNode *tn = lookup_LinearHash(linearHash, "test1");
        ck_assert_ptr_ne(tn, NULL);

        tn = lookup_LinearHash(linearHash, "xxxxxxx");
        ck_assert_ptr_eq(tn, NULL);

        LookupStruct ls = lookup_for_delete_LinearHash(linearHash, "test2");
        ck_assert_ptr_ne(ls.trieNode, NULL);

        ls = lookup_for_delete_LinearHash(linearHash, "xxxxxxx");
        ck_assert_ptr_eq(ls.trieNode, NULL);

        destroy_LinearHash(linearHash);
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
        ck_assert_int_eq(create_trie_node(trieNode), NOT_ALLOCATED_ERROR);

        trieNode = malloc(sizeof(TrieNode));
        // Create node with space
        ck_assert_int_eq(create_trie_node(trieNode), SUCCESS);
        free(trieNode);

        // TRIE CHECK
        // Create the trie
        Trie *trie = create_trie();
        ck_assert_ptr_ne(trie, NULL);
        // Insert an ngram
        ck_assert_int_eq(insert_trie(trie, tNgram), SUCCESS);

        // TODO to parakatw den douleuei
        // Check if binary search finds its first word
//        SearchResults se = binary_search(trie->root->children, "test", trie->root->occupiedPositions);
//        ck_assert_int_eq(se.found, 1);

        // These are already unit tested
        QueryResults *queryResults = create_query_results(DEFAULT_LINES, DEFAULT_LINE_SIZE);
        BloomFilter *bloomFilter = create_bloom_filter();
        NgramCounter *ngramCounter = create_ngram_counter();

        query_trie(trie, tNgram2, bloomFilter, queryResults, ngramCounter);
        copy_results_to_buffer_query_results(queryResults);
        flush_query_results(queryResults);
        // Check delete trie
        ck_assert_int_eq(delete_ngram_trie(trie, tNgram3), SUCCESS);
        ck_assert_int_eq(delete_ngram_trie(trie, tNgram4), DELETE_NOT_FOUND);
        ck_assert_int_eq(destroy_trie(trie), SUCCESS);

        destroy_query_results(queryResults);
        destroy_bloom_filter(bloomFilter);
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

Suite *linear_hash_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Linear Hash");

    // Core test case
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, unitLinearHash);
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
    Suite *linearsuite, *querySuite, *trieSuite, *counterSuite, *bloomSuite;
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

    // Run test for linear hash
    linearsuite = linear_hash_suite();
    sr = srunner_create(linearsuite);
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
