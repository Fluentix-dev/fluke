#include "parser.hpp"
#include "statements.hpp"
#include "nodes.hpp"
#include "../errors/results.hpp"
#include "expressions.hpp"
#include "../positions/positions.hpp"
#include <vector>
#include <memory>
namespace fluke {
    Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), index(0) {}
    void Parser::advance() {
        this->index++;
    }

    Result<std::shared_ptr<BlockStatement>> Parser::generate_ast() {
        std::vector<std::shared_ptr<Statement>> body = {};
        while (this->index < this->tokens.size() && this->tokens[this->index].type() != TokenType::END_OF_FILE) {
            while (this->index < this->tokens.size() && this->tokens[this->index].type() == TokenType::SEMICOLON) {
                this->advance();
            }

            if (this->index >= this->tokens.size() || this->tokens[this->index].type() == TokenType::END_OF_FILE) {
                break;
            }

            const Result<std::shared_ptr<Statement>> stmt = this->statement();
            if (!stmt.error().type().empty()) {
                return Result<std::shared_ptr<BlockStatement>>(nullptr, stmt.error());
            }

            body.push_back(stmt.result());
        }

        return Result<std::shared_ptr<BlockStatement>>(std::make_shared<BlockStatement>(this->tokens[0].start(), this->tokens[this->tokens.size()-1].end(), body), Error(this->tokens[0].start(), this->tokens[0].start(), "", ""));
    }

    Result<std::shared_ptr<Statement>> Parser::statement() {
        const Result<std::shared_ptr<Expression>> expr = this->expression();
        if (!expr.error().type().empty()) {
            return Result<std::shared_ptr<Statement>>(nullptr, expr.error());
        }

        if (this->index < this->tokens.size() && this->tokens[this->index].type() == TokenType::SEMICOLON) {
            return Result<std::shared_ptr<Statement>>(expr.result(), expr.error());
        }

        if (this->index < this->tokens.size()) {
            return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected ';', got '" + this->tokens[this->index].value() + "'"));
        }

        return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].start(), "Syntax Error", "expected ';'"));
    }

    Result<std::shared_ptr<Expression>> Parser::expression() {
        return this->additive();
    }

    Result<std::shared_ptr<Expression>> Parser::additive() {
        Result<std::shared_ptr<Expression>> lhs = this->multiplicative();
        if (!lhs.error().type().empty()) {
            return lhs;
        }

        while (this->index < this->tokens.size() && (this->tokens[this->index].type() == TokenType::PLUS || this->tokens[this->index].type() == TokenType::MINUS)) {
            const std::string op = this->tokens[this->index].value();
            this->advance();
            
            const Result<std::shared_ptr<Expression>> rhs = this->multiplicative();
            if (!rhs.error().type().empty()) {
                return rhs;
            }

            lhs = Result<std::shared_ptr<Expression>>(std::make_shared<BinaryExpression>(lhs.result()->start(), rhs.result()->end(), lhs.result(), op, rhs.result()), lhs.error());
        }

        return lhs;
    }

    Result<std::shared_ptr<Expression>> Parser::multiplicative() {
        Result<std::shared_ptr<Expression>> lhs = this->unary();
        if (!lhs.error().type().empty()) {
            return lhs;
        }

        while (this->index < this->tokens.size() && (this->tokens[this->index].type() == TokenType::MULTIPLY || this->tokens[this->index].type() == TokenType::DIVIDE)) {
            const std::string op = this->tokens[this->index].value();
            this->advance();
            
            const Result<std::shared_ptr<Expression>> rhs = this->unary();
            if (!rhs.error().type().empty()) {
                return rhs;
            }

            lhs = Result<std::shared_ptr<Expression>>(std::make_shared<BinaryExpression>(lhs.result()->start(), rhs.result()->end(), lhs.result(), op, rhs.result()), lhs.error());
        }

        return lhs;
    }

    Result<std::shared_ptr<Expression>> Parser::unary() {
        if (this->index < this->tokens.size() && (this->tokens[this->index].type() == TokenType::PLUS || this->tokens[this->index].type() == TokenType::MINUS)) {
            const Position start = this->tokens[this->index].start();
            const std::string op = this->tokens[this->index].value();
            this->advance();

            const Result<std::shared_ptr<Expression>> value = this->unary();
            if (!value.error().type().empty()) {
                return value;
            }

            return Result<std::shared_ptr<Expression>>(std::make_shared<UnaryExpression>(start, value.result()->end(), op, value.result()), value.error());
        }

        return this->primary();
    }

    Result<std::shared_ptr<Expression>> Parser::primary() {
        if (this->index < this->tokens.size()) {
            switch (this->tokens[this->index].type()) {
                case TokenType::INT: {
                    const Position start = this->tokens[this->index].start();
                    const Position end = this->tokens[this->index].end();
                    const std::string value = this->tokens[this->index].value();
                    this->advance();
                    return Result<std::shared_ptr<Expression>>(std::make_shared<IntExpression>(start, end, value), Error(this->tokens[0].start(), this->tokens[0].start(), "", ""));
                } case TokenType::FLOAT: {
                    const Position start = this->tokens[this->index].start();
                    const Position end = this->tokens[this->index].end();
                    const std::string value = this->tokens[this->index].value();
                    this->advance();
                    return Result<std::shared_ptr<Expression>>(std::make_shared<FloatExpression>(start, end, value), Error(this->tokens[0].start(), this->tokens[0].start(), "", ""));
                } case TokenType::IDENTIFIER: {
                    const Position start = this->tokens[this->index].start();   
                    const Position end = this->tokens[this->index].end();
                    const std::string symbol = this->tokens[this->index].value();
                    this->advance();
                    return Result<std::shared_ptr<Expression>>(std::make_shared<IdentifierExpression>(start, end, symbol), Error(this->tokens[0].start(), this->tokens[0].start(), "", ""));
                } case TokenType::LEFT_PAREN: {
                    const Position start = this->tokens[this->index].start();
                    this->advance();
                    const Result<std::shared_ptr<Expression>> value = this->expression();
                    if (!value.error().type().empty()) {
                        return value;
                    }

                    if (this->index < this->tokens.size() && this->tokens[this->index].type() == TokenType::RIGHT_PAREN) {
                        const Position end = this->tokens[this->index].end();
                        this->advance();
                        return Result<std::shared_ptr<Expression>>(std::make_shared<GroupExpression>(start, end, value.result()), value.error());
                    }

                    if (this->index < this->tokens.size()) {
                        return Result<std::shared_ptr<Expression>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected ')', got '" + this->tokens[this->index].value() + "'"));
                    }

                    return Result<std::shared_ptr<Expression>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].end(), "Syntax Error", "expected ')'"));
                } default:
                    return Result<std::shared_ptr<Expression>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "invalid syntax"));
            }
        }
        
        return Result<std::shared_ptr<Expression>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].start(), "Syntax Error", "invalid syntax"));
    }
}