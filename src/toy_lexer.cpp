#include <regex>
#include <cctype>
#include <vector>
#include <cstdlib>
#include <string_view>
#include "include/token.hpp"
#include "include/toy_lexer.hpp"
#include "include/error_reporter.hpp"

namespace lexer::impl {

/**
 * @brief  获取下一个词法单元
 * @return token / LexErrorPtr
 */
auto ToyLexer::nextToken() -> std::expected<token::Token, error::LexError> {
    using token::Token;
    static const std::vector<std::pair<token::Type, std::regex>> patterns {
        {token::Type::ID,  std::regex{R"(^[a-zA-Z_]\w*)"}},
        {token::Type::INT, std::regex{R"(^\d+)"}}
    };

    // 检测当前是否已经到达结尾
    if (this->pos.row >= this->text.size()) {
        return Token{token::Type::END, "#", this->pos};
    }

    auto shiftPos = [&](std::size_t delta){
        this->pos.col += delta;
        if (this->pos.col >= this->text[this->pos.row].length()) {
            ++this->pos.row;
            this->pos.col = 0;
        }
    };

    // 忽略所有空白字符
    while (this->pos.row < this->text.size()) {
        if (this->text[this->pos.row].length() == 0) {
            ++this->pos.row;
            this->pos.col = 0;
        } else if (std::isspace(this->text[this->pos.row][this->pos.col])) {
            shiftPos(1);
        } else {
            break;
        }
    }
    // 再次判断是否到结尾
    if (this->pos.row >= this->text.size()) {
        return Token{token::Type::END, "#", this->pos};
    }

    // 使用正则表达式检测 INT、ID 两类词法单元
    std::string view {this->text[this->pos.row].substr(this->pos.col)};
    for (const auto& [type, expression] : patterns) {
        std::smatch match;
        if (std::regex_search(view, match, expression)) {
            auto p = this->pos;
            shiftPos(match.length(0));
            if (type == token::Type::ID && this->keyword_table.iskeyword(match.str(0))) {
                auto keyword_type = this->keyword_table.getKeyword(match.str(0));
                return Token{keyword_type, match.str(0), p};
            }
            return Token{type, match.str(0), p};
        }
    }

    Token token       {};        // 识别到的词法单元
    char  first_char  {view[0]}; // 当前看到的第一个字符
    char  second_char {view.length() > 1 ? view[1] : '\0'}; // 当前看到的第二个字符 - 用于 lookahead

    // 检测算符和标点符号
    switch (first_char) {
    default:
        break;
    case '(':
        token = Token{token::Type::LPAREN, std::string{"("}};
        break;
    case ')':
        token = Token{token::Type::RPAREN, std::string{")"}};
        break;
    case '{':
        token = Token{token::Type::LBRACE, std::string{"{"}};
        break;
    case '}':
        token = Token{token::Type::RBRACE, std::string{"}"}};
        break;
    case '[':
        token = Token{token::Type::LBRACK, std::string{"["}};
        break;
    case ']':
        token = Token{token::Type::RBRACK, std::string{"]"}};
        break;
    case ';':
        token = Token{token::Type::SEMICOLON, std::string{";"}};
        break;
    case ':':
        token = Token{token::Type::COLON, std::string{":"}};
        break;
    case ',':
        token = Token{token::Type::COMMA, std::string{","}};
        break;
    case '+':
        token = Token{token::Type::OP_PLUS, std::string{"+"}};
        break;
    case '=':
        if (second_char == '='){
            token = Token{token::Type::OP_EQ, std::string{"=="}};
        } else {
            token = Token{token::Type::ASSIGN, std::string{"="}};
        }
        break;
    case '-':
        if (second_char == '>'){
            token = Token{token::Type::ARROW, std::string{"->"}};
        } else {
            token = Token{token::Type::OP_MINUS, std::string{"-"}};
        }
        break;
    case '*':
        if (second_char == '/'){
            token = Token{token::Type::RMUL_COM, std::string{"*/"}};
        } else {
            token = Token{token::Type::OP_MUL, std::string{"*"}};
        }
        break;
    case '/':
        if (second_char == '/'){
            token = Token{token::Type::SIN_COM, std::string{"//"}};
        } else if (second_char == '*'){
            token = Token{token::Type::LMUL_COM, std::string{"/*"}};
        } else {
            token = Token{token::Type::OP_DIV, std::string{"/"}};
        }
        break;
    case '>':
        if (second_char == '=') {
            token = Token{token::Type::OP_GE, std::string{">="}};
        } else {
            token = Token{token::Type::OP_GT, std::string{">"}};
        }
        break;
    case '<':
        if (second_char == '='){
            token = Token{token::Type::OP_LE, std::string{"<="}};
        } else {
            token = Token{token::Type::OP_LT, std::string{"<"}};
        }
        break;
    case '.':
        if(second_char == '.'){
            token = Token{token::Type::DOTS, std::string{".."}};
        } else{
            token = Token{token::Type::DOT, std::string{"."}};
        }
        break;
    case '!':
        if(second_char == '='){
            token = Token{token::Type::OP_NEQ, std::string{"!="}};
        }
        break;
    case '&':
        token = Token{token::Type::REF, std::string{"&"}};
        break;
    }

    token.setPos(this->pos);
    if (!token.getValue().empty()) {
        shiftPos(token.getValue().length());
        return token;
    }

    base::Position p = this->pos;
    shiftPos(1);

    return std::unexpected(error::LexError{
        error::LexErrorType::UnknownToken,
        "识别到未知的 token: " + view.substr(0, 1),
        p.row,
        p.col,
        view.substr(0, 1)
    });
}

/**
 * @brief 初始化关键词表
 */
void ToyLexer::initKeywordTable(void) {
    using TokenType = token::Type;
    this->keyword_table.addKeyword("if",       TokenType::IF);
    this->keyword_table.addKeyword("fn",       TokenType::FN);
    this->keyword_table.addKeyword("in",       TokenType::IN);
    this->keyword_table.addKeyword("i32",      TokenType::I32);
    this->keyword_table.addKeyword("let",      TokenType::LET);
    this->keyword_table.addKeyword("mut",      TokenType::MUT);
    this->keyword_table.addKeyword("for",      TokenType::FOR);
    this->keyword_table.addKeyword("loop",     TokenType::LOOP);
    this->keyword_table.addKeyword("else",     TokenType::ELSE);
    this->keyword_table.addKeyword("break",    TokenType::BREAK);
    this->keyword_table.addKeyword("while",    TokenType::WHILE);
    this->keyword_table.addKeyword("return",   TokenType::RETURN);
    this->keyword_table.addKeyword("continue", TokenType::CONTINUE);

    this->keyword_table.setErrReporter(this->reporter);
}

} // namespace lexer::impl
