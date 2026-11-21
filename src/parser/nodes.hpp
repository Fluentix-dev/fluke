#pragma once
#include "../positions/positions.hpp"
namespace fluke {
    enum struct NodeType {
        BLOCK_STATEMENT,
        NORMAL_VARIABLE_DECLARATION_STATEMENT,
        VARIABLE_DECLARATION_WITH_EXPRESSION_CONDITION_STATEMENT,
        IF_ELSE_STATEMENT,
        ASSIGNMENT_EXPRESSION,
        BINARY_EXPRESSION,
        UNARY_EXPRESSION,
        INT_EXPRESSION,
        FLOAT_EXPRESSION,
        IDENTIFIER_EXPRESSION,
        BOOLEAN_EXPRESSION,
        GROUPING_EXPRESSION
    };
    
    struct Statement {
    public:
        Statement(const Position &start, const Position &end, const NodeType type);
        virtual ~Statement() = default;
        Position start() const;
        Position end() const;
        NodeType type() const;
    private:
        Position start_;
        Position end_;
        NodeType type_;
    };

    struct Expression : public Statement {
        Expression(const Position &start, const Position &end, const NodeType type);
    };
}
