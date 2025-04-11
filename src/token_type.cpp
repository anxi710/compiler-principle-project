#include <iostream>
#include "include/token_type.hpp"

namespace compiler::lexer {

/**
 * @brief  TokenType 转 string
 * @param  type enum class TokenType 中的一个
 * @return 对应的 string
 */
const std::string tokenType2str(TokenType type) {
    if (type == TokenType::END) {
        return std::string{"END"};
    } else if (type == TokenType::UNKNOWN) {
        return std::string{"UNKNOWN"};
    } else if (type == TokenType::DOT) {
        return std::string{"DOT"};
    }

    std::cerr << "tokenType2str(): error termination." << std::endl;
    exit(1);
}

} // namespace compiler::lexer
