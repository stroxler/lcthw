#ifndef _darray_h
#define _darray_h

#include <stdlib.h>
#include <lcthw/dbg.h>

#define DEFAULT_EXPAND_RATE 300

typedef struct Darray {
    int end;              // length of abstract array
    int max;              // length of underlying array
    size_t element_size;
    size_t expand_rate;
    void ** contents;
} Darray;

Darray * Darray_create(size_t element_size, size_t initial_max);
void Darray_destroy(Darray * array);
void Darray_clear(Darray * array);
void Darray_clear_destroy(Darray * array);

// these both return 0 for success, -1 for error
int Darray_expand(Darray * array);
int Darray_contract(Darray * array);

// returns 0 for success, -1 for error
int Darray_push(Darray * array, void * val);
// returns the popped value
void * Darray_pop(Darray * array);

#define Darray_last(A) (A)->contents[(A)->end - 1]
#define Darray_first(A) (A)->contents[0]
#define Darray_count(A) (A)->end
#define Darray_max(A) (A)->max

// set index `i` to point to a new value
static inline void Darray_set(Darray * array, int i, void * val) {
    check(i < Darray_max(array) - 1, "Index %d out of Darray range (0, %d)",
          i, Darray_max(array) - 1);
    if (i < Darray_count(array)) {
        Darray_count = i + 1
    }
    array->contents[i] = val;
error:
    return;
}

// get the value at index `i`
static inline void * Darray_get(Darray * array, int i) {
    check(i < Darray_max(array), "Index %d out of Darray range (0, %d)",
          i, Darray_max(array));
    return array->contents[i];
error:
    return NULL;
}

// remove the value at index `i`. Do not adjust the array size, just set
// to null. Return the value.
static inline void * Darray_remove(Darray * array, int i) {
    void * val = Darray_get(array, i);
    Darray_set(array, i, NULL);
    return val;
}

// Create a new value which could be placed in the array. Allocate memory.
// You should only use `Darray_clear` if you used `Darray_newvalue` to allocate
// the values, as a rule.
static inline void * Darray_newvalue(Darray * array) {
    check(array->element_size > 0, "Can't use Darray_new on size 0 arrays");
    return calloc(1, array->element_size);
error:
    return NULL;
}

#endif
