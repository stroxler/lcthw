#ifndef _object_h
#define _object_h

#include <stdlib.h>  // size_t lives here

typedef enum {
    NORTH, SOUTH, EAST, WEST
} Direction;

typedef struct {
    char *description;
    // initialize an Object after creation (the create method, which
    // instantiates, obviously can't be a part of the struct!).
    //
    // Returns an err code; this only matters for the Map, which cretes all the
    // other objects. The Map_init err code indicates any memory errors when
    // allocating.
    int (*init)(void *self);
    // pretty print
    void (*describe)(void *self);
    // destructor (free memory)
    void (*destroy)(void *self);
    // move; returns an Object pointer of the new location as void *
    void *(*move)(void *self, Direction direction);
    // attack; retruns an indicator (used only by Map and Room, not by
    // Monster itself) to indicate whether an attack actually occurred (which
    // happens only if there's a monster in the room).
    int (*attack)(void *self, int damage);
} Object;

int Object_init(void *self);
void Object_describe(void *self);
void Object_destroy(void *self);
void *Object_move(void *self, Direction direction);

// note that the size has to be input. This wouldn't be true for a
// typical struct, but an Object is the parent type so we might be actually
// creating an object of some other size.
//
// The prototypes of the object types we create are actually Objects, so we
// know how big they are. On the other hand, the value returned by New might
// have extra fields, initialized by the init method of the prototype. This
// is why we need the size here, so we know how much to allocate.
//
// The methods of instances will actually be the entries of the proto Object
// for that type - shared among all instances. This concept, of sharing method
// pointers among all instances of a type, is the key to this style of
// protoype-based OOP.
//
// This and all other Object-returning methods have to be typed as void *
// because they need to allow pointer casting, and C only allows void pointers
// to be cast (so, for example, we can't define this as returning Object *)
void *Object_new(size_t size, Object proto, char *description);

// The code above provides all the essential functionality, but these
// macros provide the syntactic sugar to hide the internals of the prototype
// object system. Don't get too caught up on this; the implementation of
// the prototype system is more interesting than the syntactic sugar.
//
// The basic macro definition should be familiar. ## is a concatenation
// operator, so if T is, e.g., Room then T##Proto becomes RoomProto.
#define NEW(T, N) Object_new(sizeof(T), T##Proto, N)
// this lets us use object._(methodName)(arg1, arg2) to call methods.
#define _(N) proto.N

/*
 * Closing notes: hopefully you can see some obvious disadvantages
 * - you can't easily see limitations of this approach. You can't chain
 *   dot operators together easily using this object system because you have to
 *   explicitly pass self, so you'd have to chain everything twice(!).
 * - the methods have to have functionality baked-in which is only used in
 *   special cases, e.g. attack returns an int but Monster_attack doesn't use
 *   it, and init returns an int but only Map_init cares.
 * Still, it's cool to see how simple objects can work in C, and there are
 * some good examples of C syntax and semantics here.
 */

#endif
