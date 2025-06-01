#pragma once

#include <cstddef>

namespace lexer::base
{

// 为了消解头文件相互引用带来的问题，
// 需要单独提出 Position 结构，
// 而不是将其放在 lexer.hpp 或 token.hpp 中

struct Position
{
    std::size_t row = 0;
    std::size_t col = 0;

    Position() = default;
    explicit Position(std::size_t r, std::size_t c) : row(r), col(c) {}
    Position(const Position& other) = default;

    auto operator=(const Position& rhs) -> Position& = default;
};

}  // namespace lexer::base
