#pragma once
#include "../positions/positions.hpp"
#include <string>
namespace fluke {
    enum struct TokenType {
        END_OF_FILE,
        INT,
        FLOAT,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        LEFT_PAREN,
        RIGHT_PAREN,
        SEMICOLON,
        IDENTIFIER,
        EQUALS
    };

    struct Token {
    public:
        Token(const Position &start, const Position &end, const TokenType type, const std::string &value);
        Position start() const;
        Position end() const;
        TokenType type() const;
        std::string value() const;
    private:
        Position start_;
        Position end_;
        TokenType type_;
        std::string value_;
    };
}
