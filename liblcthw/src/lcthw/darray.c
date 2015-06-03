#include <assert.h>
#include <lcthw/darray.h>

Darray * Darray_create(size_t element_size, size_t initial_max) {
    Darray * array = malloc(sizeof(Darray));
    check_mem(array);
    array->max = initial_max;
    check(array->max > 0, "Cannot have non-positive initial max");
    array->contents = calloc(initial_max, sizeof(void *));
    check_mem(array->contents);
    array->end = 0;
    array->element_size = element_size;
    array->expand_rate = DEFAULT_EXPAND_RATE;
    return array;
error:
    if (array) free(array);
    return NULL;
}

void Darray_clear(Darray * array) {
    int i = 0;
    // I'm honestly a little confused about this element_size business.
    // I think it's giving us a general way of 'owning' the memory, but it's
    // not super clear.
    if (array->element_size > 0) {
        for (i = 0; i < Darray_max(array); i++) {
            if (array->contents[i] != NULL) free(array->contents[i]);
        }
    }
}

void Darray_destroy(Darray * array) {
    if (array) {
        if (array->contents) {
            // note that this isn't freeing the actual data, just hte array of
            // pointers.
            free(array->contents);
        }
        free(array);
    }
}

void Darray_clear_destroy(Darray * array) {
    Darray_clear(array);
    Darray_destroy(array);
}

static inline int Darray_resize(Darray * array, size_t newsize) {
    check(newsize > 0, "newsize must be positive");
    check(newsize > Darray_count(array),
          "newsize must be strictly larger than count");
    void * contents = realloc(array->contents, newsize * sizeof(void *));
    // we are assuming that if realloc fails, original contents are unharmed.
    check_mem(contents);
    array->max = newsize;
    array->contents = contents;
    return 0;
error:
    return -1;
}

// return 0 / -1 for error / noerror
int Darray_expand(Darray * array) {
    size_t old_max = array->max;
    check(Darray_resize(array, old_max + array->expand_rate) == 0,
          "Failed to expand array to new size %d", Darray_max(array) +
          (int) array->expand_rate);
    // zero out all the new memory....
    // I don't really understand this. I'm not totally sure that the endpoint
    // condition is quite correct, and more importantly it seems like
    // he's assuming that `expand_rate` void * pointers take up
    // `expand_rate` bytes, which I'm prety sure is not true. I think we need
    // a `sizeof(void *)` in there.
    memset(array->contents + old_max, 0, array->expand_rate + 1);
    return 0;
error:
    return -1;
}
        
// return 0 / -1 for error / noerror
int Darray_contract(Darray * array) {
    int new_size = Darray_count(array) < (int) array->expand_rate
            ? (int) array->expand_rate
            : array->end;
    return Darray_resize(array, new_size + 1);
}

// return 0 / -1 for error / noerror
int Darray_push(Darray * array, void * val) {
    // note that this is why the max needs to be bigger than the count: we
    // resize *after* adding an element.
    array->contents[array->end] = val;
    array->end++;
    if (Darray_count(array) >= Darray_max(array)) {
        int rc = Darray_expand(array);
        check(rc == 0, "Error expanding array in push");
    }
    return 0;
error:
    array->end--;
    return -1;
}

void * Darray_pop(Darray * array) {
    check(array->end - 1 > 0, "Cannot pop from an empty array");
    void * val = Darray_remove(array, array->end - 1);
    array->end--;
    if (Darray_count(array) > (int) array->expand_rate &&
        Darray_count(array) % array->expand_rate != 0) {
        Darray_contract(array);
    }
    return val;
error:
    return NULL;
}
