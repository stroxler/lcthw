#include <stdio.h>
#include <assert.h> // assert comes from here
#include <stdlib.h> // malloc and free come from here
#include <string.h> // strdup comes from here

struct Person {
    char *name;
    int age;
    int height;
    int weight;
};

// Syntax note:
// When dealing with function pointers versus functions involing pointers, *
// has lower precedence than (). So `type *func()` is equivalent to `type*
// (func())`: func returns a pointer to type. If you want a pointer to a
// function (see ex18 for an introductory example), you have to force * to have
// higher precedece by doing `type (*func)()`.
struct Person *Person_create(char *name, int age, int height, int weight) {
    struct Person *who = malloc(sizeof(struct Person));
    assert(who != NULL);
    who->name = strdup(name);
    who->age = age;
    who->height = height;
    who->weight = weight;
    return who;
}

void Person_destroy(struct Person *who) {
    assert(who != NULL);
    // note that name needs to be freed before who!
    free(who->name);
    free(who);
}

void Person_print(struct Person *who) {
    printf("Name: %s\n", who->name);
    printf("\tAge: %d\n", who->age);
    printf("\tHeight: %d\n", who->height);
    printf("\tWeight: %d\n", who->weight);
}

int main(int argc, char* argv[]) {
    struct Person *joe = Person_create("Joe Alex", 32, 64, 140);
    struct Person *frank = Person_create("Frank Blank", 20, 72, 180);

    // the %p is for pointer printing, it prints memory location in hex
    printf("Joe is at memory location %p:\n", joe);
    printf("Frank is at memory location %p:\n", frank);
    Person_print(joe);
    Person_print(frank);

    // age them
    joe->age += 20; joe->weight += 40;
    frank->age +=20; frank->height -= 2;
    Person_print(joe);
    Person_print(frank);

    // clean up
    Person_destroy(joe);
    Person_destroy(frank);
    return 0;
}
