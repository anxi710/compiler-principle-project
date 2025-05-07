#pragma once

#include <string>

namespace lexer::base {

struct Position {
    std::size_t row = 0;
    std::size_t col = 0;

    Position() = default;
    explicit Position(std::size_t r, std::size_t c) : row(r), col(c) {}
    Position(const Position& other) : row(other.row), col(other.col) {}

    Position& operator= (const Position& rhs) = default;
};

} // namespace lexer::base
