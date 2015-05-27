#include <lcthw/list_algos.h>
#include <string.h>
#include <assert.h>

#include "miniunit.h"

#define NUM_VALUES 5
char *values[] = {"XXXX", "1234", "abct", "xjvef", "NDSS"};

List *create_words() {
    int i = 0;
    List *words = List_create();
    for (i = 0; i < NUM_VALUES; i++) {
        List_push(words, values[i]);
    }
    return words;
}

int is_sorted(List * words) {
    LIST_FOREACH(words, first, next, cur) {
        if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
            debug("%s %s", (char *)cur->value, (char *)cur->next->value);
            return 0;
        }
    }
    return 1;
}

char *test_bubble_sort() {
    List * words;
    int rc;
    // test on a test list
    words = create_words();
    rc = List_bubble_sort(words, (List_compare)strcmp);
    mu_assert(rc == 0, "Bubble sort failed.");
    mu_assert(is_sorted(words), "Words should be sorted after bubble sort.")
    List_destroy(words); // note that we don't clear, b/c we don't own `values`
    // test on an empty list
    words = List_create();
    rc = List_bubble_sort(words, (List_compare)strcmp);
    mu_assert(rc == 0, "Bubble sort failed.");
    mu_assert(is_sorted(words), "Words should be sorted after bubble sort.")
    List_destroy(words); // note that we don't clear, b/c we don't own `values`
    // both tests pass
    return NULL;
}

char *test_merge_sort() {
    List * words;
    List * sorted;
    // test on a test list
    words = create_words();
    sorted = List_merge_sort(words, (List_compare)strcmp);
    mu_assert(is_sorted(sorted), "Words should be sorted after merge sort.")
    if (sorted != words) {
        List_destroy(sorted);
    }
    List_destroy(words);
    // test on an empty list
    words = List_create();
    sorted = List_merge_sort(words, (List_compare)strcmp);
    mu_assert(is_sorted(sorted), "Words should be sorted after merge sort.")
    if (sorted != words) {
        List_destroy(sorted);
    }
    List_destroy(words);
    // both tests pass
    return NULL;
}

char *all_tests() {
    mu_suite_start();
   
    mu_run_test(test_bubble_sort);
    mu_run_test(test_merge_sort);

    return NULL;
}

RUN_TESTS(all_tests);
