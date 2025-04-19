#include <cctype> // 定义有 isspace(), isalpha() 等函数
#include <vector>
#include <regex>
#include "include/token.hpp"
#include "include/toy_lexer.hpp"
#include "include/token_type.hpp"

namespace compiler::lexer {

/* constructor */

ToyLexer::ToyLexer() {
    last_matched_pos          = 0;
    longest_valid_prefix_pos  = 0;
    longest_valid_prefix_type = TokenType::UNKNOWN;

    initKeywordTable();
}

ToyLexer::ToyLexer(const std::string text) : Lexer(text) {
    ToyLexer();
}

/* constructor */

/* member function definition */

/**
 * @brief  获取下一个词法单元
 * @return  token
 */
Token ToyLexer::nextToken(void) {
    static const std::vector<std::pair<TokenType, std::regex>> patterns {
        {TokenType::ID,  std::regex{R"(^[a-zA-Z_]\w+)"}},
        {TokenType::INT, std::regex{R"(^\d+)"}}
    };

    // 检测当前是否已经到达结尾
    if (this->pos >= this->text.length()) {
        return Token::END;
    }

    // 忽略所有空字符
    while (std::isspace(this->text[this->pos]))
        ++this->pos;

    // 再次判断是否到结尾ie
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

    char firstch    =   view[0];
    char secondch   =   '\0';
    if(this->text.length() - this->pos > 1)
        secondch    =   view[1];
    //TODO 补充对算符的检测
    switch (firstch)
    {
    case '(':
        this->pos += 1;
        return {TokenType::LPAREN,"("};
        break;
           
    case ')':
        this->pos += 1;
        return {TokenType::RPAREN,")"};
        break;

    case '{':
        this->pos += 1;
        return {TokenType::LBRACE,"{"};
        break;
        
    case '}':
        this->pos += 1;
        return {TokenType::RBRACE,"}"};
        break;
    
    case '[':
        this->pos += 1;
        return {TokenType::LBRACK,"["};
        break;
    
    case ']':
        this->pos += 1;
        return {TokenType::RBRACK,"]"};   
        break;
        
    case ';':
        this->pos += 1;
        return {TokenType::SEMICOLON,";"};
        break;
    
    case ':':
        this->pos += 1;
        return {TokenType::COLON,":"};
        break;
    
    case ',':
        this->pos += 1;
        return {TokenType::COMMA,","};
        break;

    case '+':
        this->pos += 1;
        return {TokenType::OP_PLUS,"+"};
        break;
    
    case '=':
        if (secondch == '='){
            this->pos += 2;
            return {TokenType::OP_EQ,"=="}; 
        } else {
            this->pos += 1;
            return {TokenType::ASSIGN,"="};
        }
        break;
    
    case '-':
        if (secondch == '>'){
            this->pos += 2;
            return {TokenType::ARROW,"->"};
        } else {
            this->pos += 1;
            return {TokenType::OP_MINUS,"-"};
        }
        break;
    
    case '*':
        if (secondch == '/'){
            this->pos += 2;
            return {TokenType::RMUL_COM,"*/"};
        } else {
            this->pos += 1;
            return {TokenType::OP_MUL,"*"};
        }
        break;

    case '/':
        if (secondch == '/'){
            this->pos += 2;
            return {TokenType::SIN_COM,"//"};
        } else if (secondch == '*'){
            this->pos += 2;
            return {TokenType::LMUL_COM,"/*"};
        } else {
            this->pos += 1;
            return {TokenType::OP_DIV,"/"};
        }
        break;

    case '>':
        if (secondch == '=') {
            this->pos += 2;
            return {TokenType::OP_GE,">="};
        } else {
            this->pos += 1;
            return {TokenType::OP_GT,">"};
        }
        break;
    
    case '<':
        if (secondch == '='){
            this->pos += 2;
            return {TokenType::OP_LE,"<="};
        } else {
            this->pos += 1;
            return {TokenType::OP_LT,"<"};
        }
        break;

    case '.':
        if(secondch == '.'){
            this->pos += 2;
            return {TokenType::DOTS,".."};
        } else{
            this->pos += 1;
            return {TokenType::DOT,"."};
        }
        break;
    
    case '!':
        if(secondch == '='){
            this->pos += 2;
            return {TokenType::OP_NEQ,"!="};
        }

    default:
        break;
    }
    
    return Token::UNKNOWN;
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
