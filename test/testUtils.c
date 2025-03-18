#include "../src/include/utils.h"

int
main(void) {
    struct Token t = {INT};
    t.value = "10";

    printToken(1, t);

    return 0;
}
