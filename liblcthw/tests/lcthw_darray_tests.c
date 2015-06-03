#include <lcthw/darray.h>
#include "miniunit.h"

static Darray *array = NULL;
static int *val1 = NULL;
static int *val2 = NULL;

char *test_create() {
    array = Darray_create(sizeof(int), 100);
    mu_assert(array != NULL, "Darray create failed");
    mu_assert(array != NULL, "Darray contents create failed");
    mu_assert(array->max == 100, "Darray max incorrect.");
    mu_assert(array->end == 0, "Darray end incorrect.");
    mu_assert(array->element_size == sizeof(int),
              "Darray element_size wrong.");
    return NULL;
}

char *test_destroy() {
    Darray_destroy(array);
    return NULL;
}

char *test_newvalue() {
    val1 = Darray_newvalue(array);
    mu_assert(val1 != NULL, "Failed to allocate new element");
    val2 = Darray_newvalue(array);
    mu_assert(val2 != NULL, "Failed to allocate new element");
    return NULL;
}

char *test_set() {
}

char *all_tests() {
    mu_suite_start();
    mu_run_test(test_create);
    mu_run_test(test_destroy);
    return NULL;
}

RUN_TESTS(all_tests);
