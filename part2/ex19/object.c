#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "object.h"

/* This file contains generic implementations of the various Object
 * methods. Most of the types we create implement their own versions of these,
 * but when the prototype doesn't override them, these methods are the
 * defaults.
 */

void Object_destroy(void * self) {
    // note: free would work just fine without this cast. We need the cast
    // so we can free the description, not the object itself.
    Object *obj = (Object *) self;
    // remember that all free calls need to be protected against NULL
    if (obj) {
        if (obj->description) {
            free(obj->description);
        }
        free(obj);
    }
}

int Object_init(void * self) {
    return 1;
}

void Object_describe(void * self) {
    Object *obj = (Object *) self;
    printf("%s.\n", obj->description);
}

void *Object_move(void * self, Direction direction) {
    printf("You can't go in that direction.\n");
    return NULL;
}

int Object_attack(void * self, int damage) {
    printf("You can't attack that.\n");
    return 0;
}

// See object.h for a long discussion of this. We allocate memory and assign
// methods to our Object here. The size is needed because we may be casting
// to a different, possibly larger, type in the calling context.
// Note that the proto input is copied by value (although the entries are all
// pointers, so the data is shared).
void *Object_new(size_t size, Object proto, char *description) {
    // this code lets us be lazy when defining subtypes and leave all the
    // methods we aren't subtyping as null.
    if (!proto.init) proto.init = Object_init;
    if (!proto.describe) proto.describe = Object_describe;
    if (!proto.destroy) proto.destroy = Object_destroy;
    if (!proto.attack) proto.attack = Object_attack;
    if (!proto.move) proto.move = Object_move;

    // calloc is the same as malloc except
    //   (1) it takes a replication number and a size, not just a size, and
    //   (2) it zeros out the allocated bytes
    Object *obj = calloc(1, size);
    // the online code is missing this check, but I'm pretty sure the
    // `*obj = proto` is unsafe if we don't check.
    if (!obj) { printf("Error: memory error\n"); exit(1); }

    *obj = proto;  // this is a copy by value of all the pointers from proto
    obj->description = strdup(description); // copy the description
    if (!obj->init(obj)) { // init returns a failure code
        obj->destroy(obj);
        return NULL;
    } // note: Zed prefers to always have an else; this is more javascript-y
    return obj;
}
