#include <lcthw/list.h>
#include <lcthw/dbg.h>

List *List_create() {
    // remember, calloc fills out all zeros. So count is 0, ptrs are NULL.
    return calloc(1, sizeof(List));
}

// destroys a list which may not be empty (i.e. there may be nodes)
// but is already clear (i.e. all values are either null or we own other
// references to them and we don't want to collect the memory).
void List_destroy(List *list) {
    LIST_FOREACH(list, first, next, cur) {
        if (cur-> prev) {
            free(cur->prev);
        }
    }
    free(list->last);
    free(list);
}

// frees all the memory pointed to by the values
void List_clear(List *list) {
    LIST_FOREACH(list, first, next, cur) {
        free(cur->value);
        cur->value = NULL;
    }
}

// clear and destroy list
void List_clear_destroy(List *list) {
    List_clear(list);
    List_destroy(list);
}

// push to end of list
void List_push(List *list, void *value) {
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);
    node->value = value;
    if (list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }
    list->count++;
error:
    // NOTE: it seems like we actually should probably return an
    // error code. Although it's easy for users to check by looking
    // at the list length.
    return;
}

// pop from end of list
void *List_pop(List *list) {
    ListNode * node = list->last;
    return node == NULL ? NULL : List_remove(list, node);
}

// unshift to front of list
void List_unshift(List *list, void *value) {
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);
    node->value = value;
    if (list->first == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->first->prev = node;
        node->next = list->first;
        list->first = node;
    }
    list->count++;
error:
    return;
}

void *List_shift(List *list) {
    ListNode * node = list->first;
    return node == NULL ? NULL : List_remove(list, node);
}

void *List_remove(List *list, ListNode *node) {
    void *result = NULL;
    check(list->first && list ->last, "List is empty.");
    check(node, "node can't be NULL");

    if (node == list->first && node == list->last) {
        list->first = NULL;
        list->last = NULL;
    } else if (node == list->first) {
        list->first = node->next;
        check(list->first != NULL, "Invalid list, had a null element.");
        list->first->prev = NULL;
    } else if (node == list->last) {
        list->last = node->prev;
        check(list->last != NULL, "Invalid list, had a null element.");
        list->last->next = NULL;
    } else {
        ListNode *before = node->prev;
        ListNode *after = node->next;
        after->prev = before;
        before->next = after;
    }
    list->count--;
    result = node->value;
    free(node);
error:
    return result;
}

