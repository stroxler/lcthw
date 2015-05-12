#include <stdio.h>

int main(int argc, char** argv) {
    int age = 10;
    int height;

    printf("Missing variable is: %d\n");
    printf("Age is: %d\n", age);
    printf("Height is uninitialized: %d\n", height);

    return 0;
}
