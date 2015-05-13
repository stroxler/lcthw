#include <stdio.h>

int main(int argc, char** argv) {
    int distance = 100;
    float power = 2.345f;
    double super_power = 5678.99;
    char first_name[] = "Steven";
    char initial = 'L';
    char last_name[] = "Troxler";

    printf("I am %d miles away.\n", distance);
    printf("I have power %f.\n", power);
    printf("I have super power %f.\n", super_power);
    printf("My first name is %s.\n", first_name);
    printf("My middle initial is %c", initial);
    printf("My last name is %s.\n", last_name);
    printf("My full name is %s %c %s.\n", first_name, initial, last_name);

    return 0;
}
