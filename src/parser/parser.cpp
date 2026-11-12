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

    Result<std::shared_ptr<Statement>> Parser::check_semicolon(const std::shared_ptr<Statement> &stmt) const {
        if (this->index >= this->tokens.size()) {
            return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].start(), "Syntax Error", "expected ';'"));
        }

        if (this->tokens[this->index].type() == TokenType::SEMICOLON) {
            return Result<std::shared_ptr<Statement>>(stmt, Error(this->tokens[0].start(), this->tokens[0].end(), "", ""));
        }

        return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected ';', got '" + this->tokens[this->index].value() + "'"));
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
        switch (this->tokens[this->index].type()) {
            case TokenType::LET: {
                const Result<std::shared_ptr<Statement>> let_statement = this->let();
                if (!let_statement.error().type().empty()) {
                    return let_statement;
                }

                return this->check_semicolon(let_statement.result());
            } default: {
                const Result<std::shared_ptr<Expression>> expr = this->expression();
                if (!expr.error().type().empty()) {
                    return Result<std::shared_ptr<Statement>>(nullptr, expr.error());
                }

                return this->check_semicolon(expr.result());
            }
        }
    }

    Result<std::shared_ptr<Expression>> Parser::expression() {
        return this->comparative();
    }

    Result<std::shared_ptr<Expression>> Parser::comparative() {
        Result<std::shared_ptr<Expression>> lhs = this->additive();
        if (!lhs.error().type().empty()) {
            return lhs;
        }

        while (this->index < this->tokens.size() && (this->tokens[this->index].type() == TokenType::DOUBLE_EQUALS || this->tokens[this->index].type() == TokenType::NOT_EQUALS || this->tokens[this->index].type() == TokenType::GREATER_THAN || this->tokens[this->index].type() == TokenType::SMALLER_THAN || this->tokens[this->index].type() == TokenType::GREATER_THAN_OR_EQUALS || this->tokens[this->index].type() == TokenType::SMALLER_THAN_OR_EQUALS)) {
            const std::string op = this->tokens[this->index].value();
            this->advance();
            
            const Result<std::shared_ptr<Expression>> rhs = this->additive();
            if (!rhs.error().type().empty()) {
                return rhs;
            }

            lhs = Result<std::shared_ptr<Expression>>(std::make_shared<BinaryExpression>(lhs.result()->start(), rhs.result()->end(), lhs.result(), op, rhs.result()), lhs.error());
        }

        return lhs;
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

    Result<std::shared_ptr<Type>> Parser::type() {
        return this->primary_type();
    }

    Result<std::shared_ptr<Type>> Parser::primary_type() {
        if (this->index >= this->tokens.size()) {
            return Result<std::shared_ptr<Type>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].start(), "Syntax Error", "expected a type name"));
        }

        if (this->tokens[this->index].type() == TokenType::IDENTIFIER) {
            Position start = this->tokens[this->index].start();
            Position end = this->tokens[this->index].end();
            std::string type_name = this->tokens[this->index].value();
            this->advance();
            return Result<std::shared_ptr<Type>>(std::make_shared<IdentifierType>(start, end, type_name), Error(this->tokens[0].start(), this->tokens[0].start(), "", ""));
        }
        
        return Result<std::shared_ptr<Type>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected a type name, got '" + this->tokens[this->index].value() + "'"));
    }

    Result<std::shared_ptr<Statement>> Parser::let() {
        Position start = this->tokens[this->index].start();
        this->advance();
        if (this->index >= this->tokens.size()) {
            return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].end(), "Syntax Error", "expected a variable name"));
        }

        if (this->tokens[this->index].type() != TokenType::IDENTIFIER) {
            return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected a variable name, got '" + this->tokens[this->index].value() + "'"));
        }

        std::string var_name = this->tokens[this->index].value();
        this->advance();
        if (this->index >= this->tokens.size()) {
            return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].end(), "Syntax Error", "expected ':'"));
        }

        if (this->tokens[this->index].type() != TokenType::COLON) {
            return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected ':', got '" + this->tokens[this->index].value() + "'"));
        }

        this->advance();
        Result<std::shared_ptr<Type>> type = this->type();
        if (!type.error().type().empty()) {
            return Result<std::shared_ptr<Statement>>(nullptr, type.error());
        }

        if (this->index >= this->tokens.size()) {
            return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].end(), "Syntax Error", "expected ';', '=', or a condition for the variable"));
        }

        switch (this->tokens[this->index].type()) {
            case TokenType::SEMICOLON:
                return Result<std::shared_ptr<Statement>>(std::make_shared<NormalVariableDeclaration>(start, type.result()->end(), var_name, type.result(), nullptr), type.error());
            case TokenType::EQUALS: {
                this->advance();
                Result<std::shared_ptr<Expression>> value = this->expression();
                if (!value.error().type().empty()) {
                    return Result<std::shared_ptr<Statement>>(nullptr, value.error());
                }

                return Result<std::shared_ptr<Statement>>(std::make_shared<NormalVariableDeclaration>(start, value.result()->end(), var_name, type.result(), value.result()), type.error());
            } case TokenType::LEFT_PAREN: {
                this->advance();
                Result<std::shared_ptr<Expression>> condition = this->expression();
                if (!condition.error().type().empty()) {
                    return Result<std::shared_ptr<Statement>>(nullptr, condition.error());
                }

                if (this->index >= this->tokens.size()) {
                    return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].end(), "Syntax Error", "expected ')'"));
                }

                if (this->tokens[this->index].type() != TokenType::RIGHT_PAREN) {
                    return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected ')', got '" + this->tokens[this->index].value() + "'"));
                }

                this->advance();
                if (this->index >= this->tokens.size()) {
                    return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[0].start(), this->tokens[0].end(), "Syntax Error", "expected ';' or '='"));
                }

                switch (this->tokens[this->index].type()) {
                    case TokenType::SEMICOLON:
                        return Result<std::shared_ptr<Statement>>(std::make_shared<VariableDeclarationWithExpressionCondition>(start, type.result()->end(), var_name, type.result(), condition.result(), nullptr), type.error());
                    case TokenType::EQUALS: {
                        this->advance();
                        Result<std::shared_ptr<Expression>> value = this->expression();
                        if (!value.error().type().empty()) {
                            return Result<std::shared_ptr<Statement>>(nullptr, value.error());
                        }

                        return Result<std::shared_ptr<Statement>>(std::make_shared<VariableDeclarationWithExpressionCondition>(start, value.result()->end(), var_name, type.result(), condition.result(), value.result()), type.error());
                    } default:
                        return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected ';' or '='"));
                }
            } default:
                return Result<std::shared_ptr<Statement>>(nullptr, Error(this->tokens[this->index].start(), this->tokens[this->index].end(), "Syntax Error", "expected ';', '=', or a condition for the variable, got '" + this->tokens[this->index].value() + "'"));
        }
    }
}