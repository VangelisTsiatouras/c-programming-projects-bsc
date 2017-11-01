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
        //TODO unit test void function
        copyResultsToBufferQueryResults(queryResults);
        // Add a sentence when you dont have space
        ck_assert_int_eq(addLineQueryResults(queryResults, "Last sentence"), SUCCESS);
        // Print
        //TODO unit test void function
        copyResultsToBufferQueryResults(queryResults);
        flushQueryResults(queryResults);
        // Destroy
        destroyQueryResults(queryResults);
    }END_TEST;

// WIP
START_TEST(unitTestTrie)
    {
        char tNgram[] = "test ngram";
        char tNgram2[] = "test ngram";
        char tNgram3[] = "test ngram";
        char tNgram4[] = "test ngram2";
        TrieNode *trieNode = NULL;
        ck_assert_int_eq(trie_node_create(trieNode, NULL), NOT_ALLOCATED_ERROR);
        trieNode = malloc(sizeof(TrieNode));
        ck_assert_int_eq(trie_node_create(trieNode, NULL), SUCCESS);
        free(trieNode);
        Trie *trie = trie_create();
        ck_assert_ptr_ne(trie, NULL);
        ck_assert_int_eq(trie_insert(trie, tNgram), SUCCESS);
        SearchResults se = binary_search(trie->root->children, "test", trie->root->occupiedPositions);
        ck_assert_int_eq(se.found, 1);

        // This is already unit tested
        QueryResults *queryResults = createQueryResults(DEFAULT_LINES, DEFAULT_LINE_SIZE);
        trie_query(trie, tNgram2, queryResults);
        copyResultsToBufferQueryResults(queryResults);
        flushQueryResults(queryResults);
        ck_assert_int_eq(trie_delete_ngram(trie, tNgram3), SUCCESS);
        ck_assert_int_eq(trie_delete_ngram(trie, tNgram4), DELETE_NOT_FOUND);





        //ADD TESTS HERE!

    }END_TEST;

Suite *query_results_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("QueryResults");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, unitTestQueryResults);
    suite_add_tcase(s, tc_core);

    return s;
}

Suite *trie_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Trie");

    /* Core test case */
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
    // Add trie unit test here
}
