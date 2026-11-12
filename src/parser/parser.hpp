#pragma once
#include "../lexer/tokens.hpp"
#include "statements.hpp"
#include "../errors/results.hpp"
#include "types.hpp"
#include <vector>
#include <memory>
namespace fluke {
    struct Parser {
    public:
        Parser(const std::vector<Token> &tokens);
        Result<std::shared_ptr<BlockStatement>> generate_ast();
    private:
        std::vector<Token> tokens;
        size_t index;
        void advance();
        Result<std::shared_ptr<Statement>> check_semicolon(const std::shared_ptr<Statement> &stmt) const;
        Result<std::shared_ptr<Statement>> statement();
        Result<std::shared_ptr<Expression>> expression();
        Result<std::shared_ptr<Expression>> comparative();
        Result<std::shared_ptr<Expression>> additive();
        Result<std::shared_ptr<Expression>> multiplicative();
        Result<std::shared_ptr<Expression>> unary();
        Result<std::shared_ptr<Expression>> primary();
        Result<std::shared_ptr<Type>> type();
        Result<std::shared_ptr<Type>> primary_type();
        Result<std::shared_ptr<Statement>> let();
    };
}