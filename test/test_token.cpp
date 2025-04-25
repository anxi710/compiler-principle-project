#include <iostream>
#include "../src/include/token.hpp"
#include "../src/include/toy_lexer.hpp"

int main(void) {
    using lexer::token::Token;
    using lexer::impl::ToyLexer;
    ToyLexer tl {"let mut x = 42; if x > 0 { return x; } "};
    //ToyLexer tl("let mut  + ...");
    //ToyLexer tl("_x123");
    //ToyLexer tl("x123");
    //ToyLexer tl(" x");
    for(int i=20;i>0;i--)
    {
        std::cout << tl.nextToken()->toString();
    }


    return 0;
}
