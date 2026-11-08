#include "tokens.hpp"
#include "../positions/positions.hpp"
#include <string>
namespace fluke {
    Token::Token(const Position &start, const Position &end, const TokenType type, const std::string &value) : start_(start), end_(end), type_(type), value_(value) {}
    Position Token::start() const {
        return this->start_;
    }

    Position Token::end() const {
        return this->end_;
    }

    TokenType Token::type() const {
        return this->type_;
    }

    std::string Token::value() const {
        return this->value_;
    }
}
