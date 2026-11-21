#pragma once
#include "../parser/statements.hpp"
#include "../errors/errors.hpp"
#include "../parser/statements.hpp"
#include "../errors/results.hpp"
#include "../parser/types.hpp"
#include <memory>
#include <unordered_map>
#include <string>

namespace fluke {
    struct TypecheckerScope;
    struct ExpressionTypechecker {
    public:
        ExpressionTypechecker();
        Result<std::string> get_type(const std::shared_ptr<Expression> &expr, TypecheckerScope &scope) const;
        bool same_type(const std::string &type, const std::shared_ptr<Type> &expected) const;
        bool same_type(const std::string &type, const std::string &expected) const;
    private:
        std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>> operation_map;
    };

    struct TypecheckerScope {
    public:
        TypecheckerScope(const std::shared_ptr<TypecheckerScope> &parent);
        Error declare(const std::shared_ptr<Statement> &stmt, const ExpressionTypechecker &expression_typechecker);
        Result<std::string> assign(const std::shared_ptr<AssignmentExpression> &expr, const ExpressionTypechecker &expression_typechecker);
        Result<std::string> get_value(const std::shared_ptr<IdentifierExpression> &expr) const;
    private:
        std::unordered_map<std::string, std::string> type_map;
        std::shared_ptr<TypecheckerScope> parent;
    };

    struct Typechecker {
    public:
        Typechecker(const std::shared_ptr<BlockStatement> &ast);
        Error typecheck();
    private:
        std::shared_ptr<BlockStatement> ast;
        Error typecheck_(const std::shared_ptr<Statement> &stmt, std::shared_ptr<TypecheckerScope> &scope, const ExpressionTypechecker &expression_typechecker);
    };
}
