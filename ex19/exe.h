#ifndef _exe_h
#define _exe_h

#include "object.h"

// Note that in all of these types, the first entry is an Object.
// This is essential for this code to work: the generic Object code
// uses the exact same pointer. It's relying on the fact that the first
// entry in a struct has the same address as the struct itself
struct Monster {
    Object proto;
    int hit_points;
};

typedef struct Monster Monster;

int Monster_attack(void *self, int damage);
int Monster_init(void *self);

struct Room {
    Object proto;
    Monster *bad_guy;
    struct Room *north;
    struct Room *south;
    struct Room *east;
    struct Room *west;
};

typedef struct Room Room;

void *Room_move(void *self, Direction direction);
int Room_attack(void *self, int damage);

struct Map {
    Object proto;
    struct Room *start;
    struct Room *location;
};

typedef struct Map Map;

void *Map_move(void *self, Direction direction);
int Map_init(void *self);
int Map_attack(void *self, int damage);

#endif
