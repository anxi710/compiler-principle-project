#include <iostream>
#include <cstdlib>
#include <cctype>
#include <vector>
#include <regex>
#include "include/token.hpp"
#include "include/toy_lexer.hpp"
#include "include/token_type.hpp"

namespace compiler::lexer {

/* constructor */

ToyLexer::ToyLexer() : Lexer("") {
    init();
}

ToyLexer::ToyLexer(const std::string text) : Lexer(text) {
    init();
}

/* constructor */

/* member function definition */

void ToyLexer::init() {
    initKeywordTable();
}

/**
 * @brief  获取下一个词法单元
 * @return  token
 */
Token ToyLexer::nextToken(void) {
    static const std::vector<std::pair<TokenType, std::regex>> patterns {
        {TokenType::ID,  std::regex{R"(^[a-zA-Z_]\w*)"}},
        {TokenType::INT, std::regex{R"(^\d+)"}}
    };

    // 检测当前是否已经到达结尾
    if (this->pos >= this->text.length()) {
        return Token::END;
    }

    // 忽略所有空字符
    while (std::isspace(this->text[this->pos])) {
        ++this->pos;
    }

    // 再次判断是否到结尾
    if (this->pos >= this->text.length()) {
        return Token::END;
    }

    // 使用正则表达式检测 INT、ID 两类词法单元
    std::string view {this->text.substr(this->pos)};
    for (const auto& [type, expression] : patterns) {
        std::smatch match;
        if (std::regex_search(view, match, expression)) {
            this->pos += match.length(0);
            if (type == TokenType::ID && this->keyword_table.iskeyword(match.str(0))) {
                return this->keyword_table.getKeyword(match.str(0));
            }
            return {type, match.str(0)};
        }
    }

    Token token       {};        // 识别到的词法单元
    char  first_char  {view[0]}; // 当前看到的第一个字符
    char  second_char {};        // 当前看到的第二个字符 - 用于 lookahead
    if(this->text.length() - this->pos > 1) {
        second_char = view[1];
    }

    // 检测算符和标点符号
    switch (first_char) {
    default:
        break;
    case '(':
        token = Token{TokenType::LPAREN, std::string{"("}};
        break;
    case ')':
        token = Token{TokenType::RPAREN, std::string{")"}};
        break;
    case '{':
        token = Token{TokenType::LBRACE, std::string{"{"}};
        break;
    case '}':
        token = Token{TokenType::RBRACE, std::string{"}"}};
        break;
    case '[':
        token = Token{TokenType::LBRACK, std::string{"["}};
        break;
    case ']':
        token = Token{TokenType::RBRACK, std::string{"]"}};
        break;
    case ';':
        token = Token{TokenType::SEMICOLON, std::string{";"}};
        break;
    case ':':
        token = Token{TokenType::COLON, std::string{":"}};
        break;
    case ',':
        token = Token{TokenType::COMMA, std::string{","}};
        break;
    case '+':
        token = Token{TokenType::OP_PLUS, std::string{"+"}};
        break;
    case '=':
        if (second_char == '='){
            token = Token{TokenType::OP_EQ, std::string{"=="}};
        } else {
            token = Token{TokenType::ASSIGN, std::string{"="}};
        }
        break;
    case '-':
        if (second_char == '>'){
            token = Token{TokenType::ARROW, std::string{"->"}};
        } else {
            token = Token{TokenType::OP_MINUS, std::string{"-"}};
        }
        break;
    case '*':
        if (second_char == '/'){
            token = Token{TokenType::RMUL_COM, std::string{"*/"}};
        } else {
            token = Token{TokenType::OP_MUL, std::string{"*"}};
        }
        break;
    case '/':
        if (second_char == '/'){
            token = Token{TokenType::SIN_COM, std::string{"//"}};
        } else if (second_char == '*'){
            token = Token{TokenType::LMUL_COM, std::string{"/*"}};
        } else {
            token = Token{TokenType::OP_DIV, std::string{"/"}};
        }
        break;
    case '>':
        if (second_char == '=') {
            token = Token{TokenType::OP_GE, std::string{">="}};
        } else {
            token = Token{TokenType::OP_GT, std::string{">"}};
        }
        break;
    case '<':
        if (second_char == '='){
            token = Token{TokenType::OP_LE, std::string{"<="}};
        } else {
            token = Token{TokenType::OP_LT, std::string{"<"}};
        }
        break;
    case '.':
        if(second_char == '.'){
            token = Token{TokenType::DOTS, std::string{".."}};
        } else{
            token = Token{TokenType::DOT, std::string{"."}};
        }
        break;
    case '!':
        if(second_char == '='){
            token = Token{TokenType::OP_NEQ, std::string{"!="}};
        }
        break;
    }

    if (token.getValue().length() > 0) {
        this->pos += token.getValue().length();
        return token;
    } else {
        std::cerr << "ToyLexer::nextToken(): unknown token" << std::endl;
        exit(1);
    }
}

/**
 * @brief 初始化关键词表
 */
void ToyLexer::initKeywordTable(void) {
    keyword_table.addKeyword("i32",      Token::I32);
    keyword_table.addKeyword("let",      Token::LET);
    keyword_table.addKeyword("if",       Token::IF);
    keyword_table.addKeyword("else",     Token::ELSE);
    keyword_table.addKeyword("while",    Token::WHILE);
    keyword_table.addKeyword("return",   Token::RETURN);
    keyword_table.addKeyword("mut",      Token::MUT);
    keyword_table.addKeyword("fn",       Token::FN);
    keyword_table.addKeyword("for",      Token::FOR);
    keyword_table.addKeyword("in",       Token::IN);
    keyword_table.addKeyword("loop",     Token::LOOP);
    keyword_table.addKeyword("break",    Token::BREAK);
    keyword_table.addKeyword("continue", Token::CONTINUE);
}

/* member function definition */

} // namespace compiler::lexer
