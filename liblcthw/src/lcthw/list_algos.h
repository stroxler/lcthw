#ifndef lcthw_list_algos_h
#define lcthw_list_algos_h

#include <lcthw/list.h>

// comparison typedef for the void * values that we store in lists
typedef int (*List_compare)(const void *a, const void *b);

// sorts in-place. Returns 0 for success, -1 for error.
int List_bubble_sort(List *list, List_compare cmp);

// creates a new sorted list. Be careful when using this though: I don't
// think it's completely production-worthy, because it returns a reference
// to the same list if the list size is 0 or 1. This can lead to double-freeing
// problems.
List * List_merge_sort(List *list, List_compare cmp);

#endif
