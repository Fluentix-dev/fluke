#pragma once
#include "../lexer/tokens.hpp"
#include "statements.hpp"
#include "../errors/results.hpp"
#include <vector>
#include <memory>
namespace fluke {
    struct Parser {
    public:
        Parser(const std::vector<Token> &tokens);
        void advance();
        Result<std::shared_ptr<BlockStatement>> generate_ast();
    private:
        std::vector<Token> tokens;
        size_t index;
        Result<std::shared_ptr<Statement>> statement();
        Result<std::shared_ptr<Expression>> expression();
        Result<std::shared_ptr<Expression>> additive();
        Result<std::shared_ptr<Expression>> multiplicative();
        Result<std::shared_ptr<Expression>> unary();
        Result<std::shared_ptr<Expression>> primary();
    };
}