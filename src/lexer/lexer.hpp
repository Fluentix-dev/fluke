#pragma once
#include "../positions/positions.hpp"
#include "tokens.hpp"
#include "../errors/results.hpp"
#include <vector>
namespace fluke {
    struct Lexer {
    public:
        Lexer(const std::string &file_name, const std::string &src);
        Result<std::vector<Token>> tokenize();
    private:
        std::string src;
        size_t index;
        Position position;
        void advance();
    };
}
