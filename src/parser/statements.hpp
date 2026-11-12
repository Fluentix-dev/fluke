#pragma once
#include "nodes.hpp"
#include "types.hpp"
#include "expressions.hpp"
#include <vector>
#include <memory>
#include <string>
namespace fluke {
    struct BlockStatement : public Statement {
    public:
        BlockStatement(const Position &start, const Position &end, const std::vector<std::shared_ptr<Statement>> &body);
        std::vector<std::shared_ptr<Statement>> body() const;
    private:
        std::vector<std::shared_ptr<Statement>> body_;
    };

    struct NormalVariableDeclaration : public Statement {
    public:
        NormalVariableDeclaration(const Position &start, const Position &end, const std::string &var_name, const std::shared_ptr<Type> &var_type, const std::shared_ptr<Expression> &value);
        std::string var_name() const;
        std::shared_ptr<Type> var_type() const;
        std::shared_ptr<Expression> value() const;
    private:
        std::string var_name_;
        std::shared_ptr<Type> var_type_;
        std::shared_ptr<Expression> value_;
    };

    struct VariableDeclarationWithExpressionCondition : public Statement {
    public:
        VariableDeclarationWithExpressionCondition(const Position &start, const Position &end, const std::string &var_name, const std::shared_ptr<Type> &var_type, const std::shared_ptr<Expression> &condition, const std::shared_ptr<Expression> &value);
        std::string var_name() const;
        std::shared_ptr<Type> var_type() const;
        std::shared_ptr<Expression> condition() const;
        std::shared_ptr<Expression> value() const;
    private:
        std::string var_name_;
        std::shared_ptr<Type> var_type_;
        std::shared_ptr<Expression> condition_;
        std::shared_ptr<Expression> value_;
    };
}