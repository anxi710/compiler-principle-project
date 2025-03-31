#include <stdio.h>
#include "../src/include/token.h"

extern void printToken(FILE*, struct Token);

int
main(void) {
    struct Token t = {INT, "10"};

    printToken(stdout, t);

    return 0;
}
