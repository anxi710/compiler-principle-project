#include "token.hpp"

#include <sstream>

namespace lexer::token
{

/* function member definition */

/**
 * @brief  将 token 格式化为一个 string
 * @return 格式化后的字符串
 */
auto Token::toString() const -> std::string
{
    std::ostringstream oss;
    oss << "<type: " << tokenType2str(type) << ", value: \"" << value << "\">@("
        << this->pos.row + 1 << ", " << this->pos.col + 1 << ")";
    return oss.str();
}

/* function member definition */

}  // namespace lexer::token
