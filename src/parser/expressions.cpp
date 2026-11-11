#include "expressions.hpp"
#include "nodes.hpp"
#include <memory>
#include <string>
#include <utility>
namespace fluke {
    BinaryExpression::BinaryExpression(const Position &start, const Position &end, const std::shared_ptr<Expression> &lhs, const std::string &op, const std::shared_ptr<Expression> &rhs) : Expression(start, end, NodeType::BINARY_EXPRESSION), lhs_(lhs), op_(op), rhs_(rhs) {}
    std::shared_ptr<Expression> BinaryExpression::lhs() const {
        return this->lhs_;
    }

    std::string BinaryExpression::op() const {
        return this->op_;
    }

    std::shared_ptr<Expression> BinaryExpression::rhs() const {
        return this->rhs_;
    }

    UnaryExpression::UnaryExpression(const Position &start, const Position &end, const std::string &op, const std::shared_ptr<Expression> &value) : Expression(start, end, NodeType::UNARY_EXPRESSION), op_(op), value_(value) {}
    std::string UnaryExpression::op() const {
        return this->op_;
    }

    std::shared_ptr<Expression> UnaryExpression::value() const {
        return this->value_;
    }
    
    IntExpression::IntExpression(const Position &start, const Position &end, const std::string &value) : Expression(start, end, NodeType::INT_EXPRESSION), value_(value) {}
    std::string IntExpression::value() const {
        return this->value_;
    }

    FloatExpression::FloatExpression(const Position &start, const Position &end, const std::string &value) : Expression(start, end, NodeType::FLOAT_EXPRESSION), value_(value) {}
    std::string FloatExpression::value() const {
        return this->value_;
    }

    IdentifierExpression::IdentifierExpression(const Position &start, const Position &end, const std::string &symbol) : Expression(start, end, NodeType::IDENTIFIER_EXPRESSION), symbol_(symbol) {}
    std::string IdentifierExpression::symbol() const {
        return this->symbol_;
    }

    GroupExpression::GroupExpression(const Position &start, const Position &end, const std::shared_ptr<Expression> &value) : Expression(start, end, NodeType::GROUPING_EXPRESSION), value_(value) {}
    std::shared_ptr<Expression> GroupExpression::value() const {
        return this->value_;
    }
}
