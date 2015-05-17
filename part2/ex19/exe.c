#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "exe.h"


// general game layout: all user interactions are delegated from the Map to
// Rooms and then in turn to Monsters.
//
// The Map sets up the underlying topology (Map_init is what creates all the
// Rooms and Monsters) and tracks the player position.
//
// The user interaction is handled by process_input.

// Note: if you switch to C++ programming, this type of implicit cast
// won't work. To do Monster *monster = self, you would need to explicitly
// cast: Monster *monster = (monster *) self;
int Monster_attack(void *self, int damage) {
    Monster *monster = self;
    printf("You attack the %s!\n", monster->_(description));
    monster->hit_points -= damage;
    if (monster->hit_points > 0) {
        printf("It is still alive.\n");
    } else {
        printf("It is dead!");
    }
}

int Monster_init(void *self) {
    Monster *monster = self;
    monster->hit_points = 10;
}

Object MonsterProto = {
    .init = Monster_init,
    .attack = Monster_attack
};

void *Room_move(void *self, Direction direction) {
    Room *room = self;
    Room *next = NULL;
    char *directionString;
    // set next
    if (direction == NORTH && room->north) {
        next = room->north;
        directionString = "north";
    } else if (direction == SOUTH && room->south) {
        next = room->south;
        directionString = "south";
    } else if (direction == EAST && room->east) {
        next = room->east;
        directionString = "east";
    } else if (direction == WEST && room->west) {
        next = room->west;
        directionString = "west";
    } else {
        next = NULL;
    }
    // report whether the movement worked
    if (next) {
        printf("You go %s, into:\n", directionString);
        next->_(describe)(next);
    } else {
        printf("You can't go that direction.");
    }
    return next;
}

int Room_attack(void *self, int damage) {
    Room *room = self;
    Monster *monster = room->bad_guy;
    if (monster) {
        monster->_(attack)(monster, damage);
        return 1;
    } else {
        printf("You flail at the emtpy air, silly.\n");
        return 0;
    }
}

Object RoomProto = {
    .move = Room_move,
    .attack = Room_attack,
};

void *Map_move(void *self, Direction direction) {
    Map *map = self;
    Room *location = map->location;
    Room *next = NULL;
    next = location->_(move)(location, direction);
    // we may not have moved; change the location if we did
    if (next) {
        map->location = next;
    }
    return next;
}

int Map_attack(void *self, int damage) {
    Map *map = self;
    Room *location = map->location;
    return location->_(attack)(location, damage);
}

int Map_init(void *self) {
    Map *map = self;

    // make some rooms for a small map
    Room *hall = NEW(Room, "The great Hall");
    Room *throne = NEW(Room, "The throne room");
    Room *arena = NEW(Room, "The arena, with the minotaur");
    Room *kitchen = NEW(Room, "Kitchen, you have the knife now");

    // put the bad guy in the arena
    arena->bad_guy = NEW(Monster, "The evil minotaur");

    // setup the map rooms
    hall->north = throne;

    throne->west = arena;
    throne->east = kitchen;
    throne->south = hall;

    arena->east = throne;
    kitchen->west = throne;

    // start the map and the character off in the hall
    map->start = hall;
    map->location = hall;

    return 1;
}

Object MapProto = {
    .init = Map_init,
    .move = Map_move,
    .attack = Map_attack,
};

// The return code is to handle the end of the game: the loop in
// main terminates when we return 0.
int process_input(Map *game) {

    printf("\n> ");
    char ch = getchar();
    getchar(); // eat ENTER
    int damage = rand() % 4;

    switch(ch) {
        // Here we are explicitly using that EOF is -1
        case -1:
            printf("Giving up? You suck.\n");
            return 0;
            break;
        // list available directions (l)
        case 'l':
            printf("You can go:\n");
            if(game->location->north) printf("NORTH\n");
            if(game->location->south) printf("SOUTH\n");
            if(game->location->east) printf("EAST\n");
            if(game->location->west) printf("WEST\n");
            break;
        // directions (n, s, e, w)
        case 'n':
            game->_(move)(game, NORTH);
            break;
        case 's':
            game->_(move)(game, SOUTH);
            break;
        case 'e':
            game->_(move)(game, EAST);
            break;
        case 'w':
            game->_(move)(game, WEST);
            break;
        // attack (a)
        case 'a':
            game->_(attack)(game, damage);
            break;
        // error for any unknown command
        default:
            printf("What?: %d\n", ch);
    }
    return 1;
}

int main(int argc, char **argv) {
    // set the random seed
    srand(time(NULL));
    // set up the map... note that init isn't called explicitly because it's
    // called inside Object_new.
    Map *game = NEW(Map, "The Hall of the Minotaur.");
    // print initial location
    printf("You enter the ");
    game->location->_(describe)(game->location);

    // the actual game runs in a while loop. Note the empty body.
    while (process_input(game)) {}

    return 0;
}
