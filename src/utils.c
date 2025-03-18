// 实现 utils.h 中声明的实用函数

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/utils.h"

void
printToken(int fd, struct Token token) {
    FILE* out = fdopen(fd, "a");

    char* type;
    switch(token.type) {
        case INT:
            type = (char*)malloc(4);
            strcpy(type, "INT");
            break;
        default:
            dprintf(2, "printToken: invalid token type");
            exit(1);
    }

    fprintf(out, "<type, value>: <%s, %s>", type, token.value);

    return;
}
