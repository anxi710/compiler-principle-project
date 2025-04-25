#include <vector>
#include <string>
#include <cassert>
#include <iostream>
#include "../src/include/keyword_table.hpp"

int main(void) {
    using lexer::keyword::KeywordTable;
    using lexer::token::Type;
    using lexer::token::Token;

    KeywordTable keyword_table;
    keyword_table.addKeyword("let", Token::LET);
    keyword_table.addKeyword("mut", Token::MUT);
    keyword_table.addKeyword("i32", Token::I32);

    std::vector<std::string> vec {"let", "mut", "i32"};

    for (const auto& str : vec) {
        assert(keyword_table.iskeyword(str));
        std::cout << keyword_table.getKeyword(str).toString();
    }

    return 0;
}