#ifndef _lcthw_list_h
#define _lcthw_list_h

#include <stdlib.h>


// for structs that contain themselves, you need to declare them before
// you define them!

struct ListNode;

typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    void *value;
} ListNode;

typedef struct List {
    int count;
    ListNode *first;
    ListNode *last;
} List;

List *List_create();
void List_destroy(List *list);
void List_clear(List *list);
void List_clear_destroy(List *list);

#define List_count(A) ((A)->count)
#define List_first(A) ((A)->first == NULL ? NULL : (A)->first->value)
#define List_last(A) ((A)->last == NULL ? NULL : (A)->last->value)

void List_push(List *list, void *value);
void *List_pop(List *list);
void List_unshift(List *list, void *value);
void *List_shift(List *list);

void *List_remove(List *list, ListNode *ListNode);

// Here L is a List*, V is a placeholder name for a ListNode* (doesn't
// need to be declared, this loop code does it for you), S is either first
// or last, and M is either next or prev.
//   Note that becuause we use _node to track our place, it's okay to reassign
// V inside of the loop. Although if you reassign V->*, then things will get
// interesting.
#define LIST_FOREACH(L, S, M, V) ListNode *_node = NULL; \
                                 ListNode *V     = NULL; \
                                 for(V = _node = L->S; _node != NULL; \
                                     V = _node = _node->M)

#endif
