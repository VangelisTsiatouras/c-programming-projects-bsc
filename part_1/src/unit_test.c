#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

// Use "Check" framework
#include <check.h>
#include <stdint.h>

#include "trie.h"
#include "queryresults.h"
#include "defs.h"

START_TEST (unitTestQueryResults)
    {
        QueryResults *queryResults;
        queryResults = createQueryResults(3, 20);
        ck_assert_int_eq(addLineQueryResults(queryResults, "Test sentence"), SUCCESS);
        ck_assert_int_eq(addLineQueryResults(queryResults, "Second sentence"), SUCCESS);
        // Add a big sentence
        ck_assert_int_eq(
                addLineQueryResults(queryResults, "This is a big line. This is a big line. This is a big line"),
                SUCCESS);
        copyResultsToBufferQueryResults(queryResults);
        // Add a sentence when you dont have space
        ck_assert_int_eq(addLineQueryResults(queryResults, "Last sentence"), SUCCESS);
        // Print
        copyResultsToBufferQueryResults(queryResults);
        flushQueryResults(queryResults);
        // Destroy
        destroyQueryResults(queryResults);
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

        // QueryResults is already unit tested
        QueryResults *queryResults = createQueryResults(DEFAULT_LINES, DEFAULT_LINE_SIZE);
        trie_query(trie, tNgram2, queryResults);
        copyResultsToBufferQueryResults(queryResults);
        flushQueryResults(queryResults);
        // Check delete trie
        ck_assert_int_eq(trie_delete_ngram(trie, tNgram3), SUCCESS);
        ck_assert_int_eq(trie_delete_ngram(trie, tNgram4), DELETE_NOT_FOUND);

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
    Suite *querySuite, *trieSuite;
    SRunner *sr;

    // Run test for query results
    querySuite = query_results_suite();
    sr = srunner_create(querySuite);
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
