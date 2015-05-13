#include <stdio.h>

int main(int argc, char* argv[]) {
    int bugs = 100;
    double bug_rate = 1.2;
    long total_bugs = 1L * 1024L * 1024L * 1024L;

    printf("You have %d bugs at the imaginary rate of %f.\n", bugs, bug_rate);
    printf("The universe has %ld bugs.\n", total_bugs);

    double expected_bugs = bugs * bug_rate;
    printf("You are expected to have %f bugs.\n", expected_bugs);

    double fraction_of_total = expected_bugs / total_bugs;
    printf("This is only a %e portion of the total.\n", fraction_of_total);

    char nul_byte = '\0';
    int care_percentage = bugs * nul_byte;
    printf("All of this means you should care %d%%.\n", care_percentage);

    return 0;
}
