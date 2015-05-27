#include <lcthw/list_algos.h>
#include <lcthw/dbg.h>
#include <assert.h>

// utility function for List_bubble_sort
inline void ListNode_swapValues(ListNode * a, ListNode * b) {
    void * tmp = a->value;
    a->value = b->value;
    b->value = tmp;
}

int List_bubble_sort(List *list, List_compare cmp) {
    int n = List_count(list);
    int i = 0;
    for (i = 0; i < n; i++) {
        LIST_FOREACH(list, first, next, curr) {
            if (curr->next && cmp(curr->value, curr->next->value) > 0) {
                ListNode_swapValues(curr, curr->next);
            }
        }
    }
    return 0;
}

inline List * List_merge(List *sorted0, List *sorted1, List_compare cmp) {
    List * sorted = List_create();
    while (List_count(sorted0) > 0 || List_count(sorted1) > 0) {
        if (List_count(sorted0) > 0 && List_count(sorted1) > 0) {
            if (cmp(List_first(sorted0), List_first(sorted1)) <= 0) {
                List_push(sorted, List_shift(sorted0));
            } else {
                List_push(sorted, List_shift(sorted1));
            }
        } else if (List_count(sorted0) > 0) {
            List_push(sorted, List_shift(sorted0));
        } else {
            List_push(sorted, List_shift(sorted1));
        }
    }
    return sorted;
}

List * List_merge_sort(List *list, List_compare cmp) {
    // handle the base case: list of size 1 or 0
    if (List_count(list) <= 1) {
        return list;
    }
    // instantiate
    List *list0, *list1, *sorted0, *sorted1, *sorted;
    int counter = List_count(list) / 2;
    // split list
    list0 = List_create();
    list1 = List_create();
    LIST_FOREACH(list, first, next, curr) {
        if (counter > 0) {
            List_push(list0, curr->value);
        } else {
            List_push(list1, curr->value);
        }
        counter--;
    }
    sorted0 = List_merge_sort(list0, cmp);
    sorted1 = List_merge_sort(list1, cmp);
    sorted = List_merge(sorted0, sorted1, cmp);
    if (sorted0 != list0) {
        List_destroy(sorted0);
    }
    List_destroy(list0);
    if (sorted1 != list1) {
        List_destroy(sorted1);
    }
    List_destroy(list1);
    // all done!
    return sorted;
}

void List_print(List *list, char *msg) {
    log_info("list >> %s", msg);
    LIST_FOREACH(list, first, next, curr) {
        log_info("\t%s", curr->value);
    }
}


/* For reference, this was my original implementation.
 *   I tried to swap actual nodes rather than values. In the cases I've been
 *   able to track this works, but there's a memory corruption somewhere.
 *   As you can see, swaping the actual nodes is quite a bit harder to do
 *   correctly than swapping values.
 */
inline void ListNode_swapNodes(ListNode * oldfirst, ListNode * oldnext) {
    assert(oldfirst->next == oldnext && oldnext->prev == oldfirst);
    // handle left side
    if (oldfirst->prev) {
        oldfirst->prev->next = oldnext;
    }
    oldnext->prev = oldfirst->prev;
    // handle right side
    if (oldnext->next) {
        oldnext->next->prev = oldfirst;
    }
    oldfirst->next = oldnext->next;
    // handle the middle
    oldnext->next = oldfirst;
    oldfirst->prev = oldnext;
}

int List_bubble_sort_swapNodes(List *list, List_compare cmp) {
    int n = List_count(list);
    int i = 0;
    for (i = 0; i < n; i++) {
        printf("i is %d\n", i);
        LIST_FOREACH(list, first, next, curr) {
            if (curr->next && cmp(curr->value, curr->next->value) > 0) {
                log_info("cur is           %p", curr);
                log_info("cur->prev is     %p", curr->prev);
                log_info("cur->next is     %p", curr->next);
                ListNode_swapNodes(curr, curr->next);
                log_info("------------------- after the swap...");
                log_info("cur is           %p", curr);
                log_info("cur->prev is     %p", curr->prev);
                log_info("cur->next is     %p", curr->next);
            }
        }
    }
    return 0;
}

