#include "statements.hpp"
#include "nodes.hpp"
#include "types.hpp"
#include "expressions.hpp"
#include <vector>
#include <memory>
#include <string>
namespace fluke {
    BlockStatement::BlockStatement(const Position &start, const Position &end, const std::vector<std::shared_ptr<Statement>> &body) : Statement(start, end, NodeType::BLOCK_STATEMENT), body_(body) {}
    std::vector<std::shared_ptr<Statement>> BlockStatement::body() const {
        return this->body_;
    }

    NormalVariableDeclaration::NormalVariableDeclaration(const Position &start, const Position &end, const std::string &var_name, const std::shared_ptr<Type> &var_type, const std::shared_ptr<Expression> &value) : Statement(start, end, NodeType::NORMAL_VARIABLE_DECLARATION_STATEMENT), var_name_(var_name), var_type_(var_type), value_(value) {}
    std::string NormalVariableDeclaration::var_name() const {
        return this->var_name_;
    }

    std::shared_ptr<Type> NormalVariableDeclaration::var_type() const {
        return this->var_type_;
    }

    std::shared_ptr<Expression> NormalVariableDeclaration::value() const {
        return this->value_;
    }

    VariableDeclarationWithExpressionCondition::VariableDeclarationWithExpressionCondition(const Position &start, const Position &end, const std::string &var_name, const std::shared_ptr<Type> &var_type, const std::shared_ptr<Expression> &condition, const std::shared_ptr<Expression> &value) : Statement(start, end, NodeType::VARIABLE_DECLARATION_WITH_EXPRESSION_CONDITION_STATEMENT), var_name_(var_name), var_type_(var_type), condition_(condition), value_(value) {}
    std::string VariableDeclarationWithExpressionCondition::var_name() const {
        return this->var_name_;
    }

    std::shared_ptr<Type> VariableDeclarationWithExpressionCondition::var_type() const {
        return this->var_type_;
    }

    std::shared_ptr<Expression> VariableDeclarationWithExpressionCondition::condition() const {
        return this->condition_;
    }

    std::shared_ptr<Expression> VariableDeclarationWithExpressionCondition::value() const {
        return this->value_;
    }
}