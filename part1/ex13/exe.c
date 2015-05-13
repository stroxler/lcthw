#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <one_word>\n", argv[0]);
        return 1;
    }

    /* Note the looping condition: it doesn't have to have the form i < n */
    int i;
    for (i = 0; argv[1][i] != '\0'; i++) {
        char letter = argv[1][i];
        switch (letter) {
            case 'a':
            case 'A':
                printf("%d: 'A'\n", i);
                break;

            case 'e':
            case 'E':
                printf("%d: 'E'\n", i);
                break;

            case 'i':
            case 'I':
                printf("%d: 'I'\n", i);
                break;

            case 'o':
            case 'O':
                printf("%d: 'O'\n", i);
                break;

            case 'u':
            case 'U':
                printf("%d: 'U'\n", i);
                break;

            case 'y':
            case 'Y':
                if(i > 2) {
                    // it's only sometimes Y
                    printf("%d: 'Y'\n", i);
                }
                break;
            default:
                printf("%d: %c is not a vowel\n", i, letter);
        }
    }
    return 0;
}
