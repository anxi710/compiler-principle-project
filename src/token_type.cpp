#include "include/token_type.hpp"

const std::string tokenType2str(TokenType type) {
    if (type == TokenType::END) {
        return std::string{"END"};
    } else if (type == TokenType::UNKNOWN) {
        return std::string{"UNKNOWN"};
    } else if (type == TokenType::DOT) {
        return std::string{"DOT"};
    }

    std::cerr << "tokenType2str(): error termination." << std::endl;
    exit(-1);
}