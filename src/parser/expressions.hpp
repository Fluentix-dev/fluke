#pragma once
#include "nodes.hpp"
#include "../positions/positions.hpp"
#include <memory>
#include <string>
namespace fluke {
    struct BinaryExpression : public Expression {
    public:
        BinaryExpression(const Position &start, const Position &end, const std::shared_ptr<Expression> &lhs, const std::string &op, const std::shared_ptr<Expression> &rhs);
        std::shared_ptr<Expression> lhs() const;
        std::string op() const;
        std::shared_ptr<Expression> rhs() const;
    private:
        std::shared_ptr<Expression> lhs_;
        std::string op_;
        std::shared_ptr<Expression> rhs_;
    };

    struct UnaryExpression : public Expression {
    public:
        UnaryExpression(const Position &start, const Position &end, const std::string &op, const std::shared_ptr<Expression> &value);
        std::string op() const;
        std::shared_ptr<Expression> value() const;
    private:
        std::string op_;
        std::shared_ptr<Expression> value_;
    };

    struct IntExpression : public Expression {
    public:
        IntExpression(const Position &start, const Position &end, const std::string &value);
        std::string value() const;
    private:
        std::string value_;
    };

    struct FloatExpression : public Expression {
    public:
        FloatExpression(const Position &start, const Position &end, const std::string &value);
        std::string value() const;
    private:
        std::string value_;
    };

    struct IdentifierExpression : public Expression {
    public:
        IdentifierExpression(const Position &start, const Position &end, const std::string &symbol);
        std::string symbol() const;
    private:
        std::string symbol_;
    };

    struct GroupExpression : public Expression {
    public:
        GroupExpression(const Position &start, const Position &end, const std::shared_ptr<Expression> &value);
        std::shared_ptr<Expression> value() const;
    private:
        std::shared_ptr<Expression> value_;
    };
}
