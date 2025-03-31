// 实现 utils.h 中声明的实用函数
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "include/token.h"
#include "include/utils.h"

void
printToken(FILE* out, struct Token token) {
    assert(out != NULL);

    const char* type;
    switch(token.type) {
        case INT:
            type = "INT";
            break;
        default:
            fprintf(stderr, "printToken: invalid token type\n");
            exit(1);
    }

    fprintf(out, "<type, value>: <%s, %s>", type, token.value);
    fflush(out);

    return;
}
