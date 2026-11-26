#include "typechecker.hpp"
#include "../parser/statements.hpp"
#include "../parser/nodes.hpp"
#include "../errors/errors.hpp"
#include "../parser/expressions.hpp"
#include "../errors/results.hpp"
#include "../parser/types.hpp"
#include <cstddef>
#include <memory>
#include <string>
namespace fluke {
    ExpressionTypechecker::ExpressionTypechecker() {
        this->operation_map = {};
        for (size_t i = 0; i < 4; i++) {
            std::string op;
            switch (i) {
                case 0:
                    op = "+";
                    break;
                case 1:
                    op = "-";
                    break;
                case 2:
                    op = "*";
                    break;
                case 3:
                    op = "/";
                    break;
            }

            for (size_t j = 0; j < 3; j++) {
                std::string type1;
                switch (j) {
                    case 0:
                        type1 = "int";
                        break;
                    case 1:
                        type1 = "float";
                        break;
                    case 2:
                        type1 = "bool";
                        break;
                }

                for (size_t k = 0; k < 3; k++) {
                    std::string type2;
                    switch (k) {
                        case 0:
                            type2 = "int";
                            break;
                        case 1:
                            type2 = "float";
                            break;
                        case 2:
                            type2 = "bool";
                            break;
                    }
                    
                    this->operation_map[type1][op][type2] = (type1 == "float" || type2 == "float" ? "float" : "int");
                }
            }
        }

        for (size_t i = 0; i < 3; i++) {
            std::string type;
            switch (i) {
                case 0:
                    type = "int";
                    break;
                case 1:
                    type = "float";
                    break;
                case 2:
                    type = "bool";
                    break;
            }

            for (size_t j = 0; j < 6; j++) {
                std::string op;
                switch (j) {
                    case 0:
                        op = "==";
                        break;
                    case 1:
                        op = "!=";
                        break;
                    case 2:
                        op = ">";
                        break;
                    case 3:
                        op = ">=";
                        break;
                    case 4:
                        op = "<";
                        break;
                    case 5:
                        op = "<=";
                        break;
                }

                this->operation_map[type][op][type] = "bool";
            }
        }

        this->operation_map["bool"]["&&"]["bool"] = "bool";
        this->operation_map["bool"]["||"]["bool"] = "bool";
        this->operation_map["<unary>"]["!"]["bool"] = "bool";
        for (size_t i = 0; i < 3; i++) {
            std::string type;
            switch (i) {
                case 0:
                    type = "int";
                    break;
                case 1:
                    type = "float";
                    break;
                case 2:
                    type = "bool";
                    break;
            }

            for (size_t j = 0; j < 2; j++) {
                const std::string op = (j == 0 ? "+" : "-");
                this->operation_map["<unary>"][op][type] = (type == "float" ? "float" : "int");
            }
        }
    }

    Result<std::string> ExpressionTypechecker::get_type(const std::shared_ptr<Expression> &expr, TypecheckerScope &scope) const {
        switch (expr->type()) {
            case NodeType::GROUPING_EXPRESSION:
                return this->get_type(std::static_pointer_cast<GroupExpression>(expr)->value(), scope);
            case NodeType::BINARY_EXPRESSION: {
                const std::shared_ptr<BinaryExpression> binary_expression = std::static_pointer_cast<BinaryExpression>(expr);
                const Result<std::string> lhs = this->get_type(binary_expression->lhs(), scope);
                if (!lhs.error().type().empty()) {
                    return lhs;
                }

                const Result<std::string> rhs = this->get_type(binary_expression->rhs(), scope);
                if (!rhs.error().type().empty()) {
                    return rhs;
                }

                if (!this->operation_map.count(lhs.result())) {
                    return Result<std::string>("", Error(expr->start(), expr->end(), "Type Error", "there exists no operation '" + binary_expression->op() + "' between '" + lhs.result() + "' and '" + rhs.result() + "'"));
                }

                if (!this->operation_map.at(lhs.result()).count(binary_expression->op())) {
                    return Result<std::string>("", Error(expr->start(), expr->end(), "Type Error", "there exists no operation '" + binary_expression->op() + "' between '" + lhs.result() + "' and '" + rhs.result() + "'"));
                }

                if (!this->operation_map.at(lhs.result()).at(binary_expression->op()).count(rhs.result())) {
                    return Result<std::string>("", Error(expr->start(), expr->end(), "Type Error", "there exists no operation '" + binary_expression->op() + "' between '" + lhs.result() + "' and '" + rhs.result() + "'"));
                }

                return Result<std::string>(this->operation_map.at(lhs.result()).at(binary_expression->op()).at(rhs.result()), lhs.error());
            } case NodeType::UNARY_EXPRESSION: {
                const std::shared_ptr<UnaryExpression> unary_expression = std::static_pointer_cast<UnaryExpression>(expr);
                const Result<std::string> value = this->get_type(unary_expression->value(), scope);
                if (!value.error().type().empty()) {
                    return value;
                }

                if (!this->operation_map.count("<unary>")) {
                    return Result<std::string>("", Error(expr->start(), expr->end(), "Type Error", "there exists no operation '" + unary_expression->op() + "' on '" + value.result() + "'"));
                }

                if (!this->operation_map.at("<unary>").count(unary_expression->op())) {
                    return Result<std::string>("", Error(expr->start(), expr->end(), "Type Error", "there exists no operation '" + unary_expression->op() + "' on '" + value.result() + "'"));
                }

                if (!this->operation_map.at("<unary>").at(unary_expression->op()).count(value.result())) {
                    return Result<std::string>("", Error(expr->start(), expr->end(), "Type Error", "there exists no operation '" + unary_expression->op() + "' on '" + value.result() + "'"));
                }

                return Result<std::string>(this->operation_map.at("<unary>").at(unary_expression->op()).at(value.result()), value.error());
            } case NodeType::INT_EXPRESSION:
                return Result<std::string>("int", Error(expr->start(), expr->start(), "", ""));
            case NodeType::FLOAT_EXPRESSION:
                return Result<std::string>("float", Error(expr->start(), expr->start(), "", ""));
            case NodeType::IDENTIFIER_EXPRESSION: {
                return scope.get_value(std::static_pointer_cast<IdentifierExpression>(expr));
            } case NodeType::BOOLEAN_EXPRESSION: {
                return Result<std::string>("bool", Error(expr->start(), expr->start(), "", ""));
            } default:
                return Result<std::string>("", Error(expr->start(), expr->end(), "Typechecker Error", "some invalid expression bs: '" + std::to_string(static_cast<int>(expr->type())) + "'"));
        }
    }

    bool ExpressionTypechecker::same_type(const std::string &type, const std::shared_ptr<Type> &expected) const {
        switch (expected->type()) {
            case TypeType::IDENTIFIER: {
                const std::shared_ptr<IdentifierType> identifier_type = std::static_pointer_cast<IdentifierType>(expected);
                return type == identifier_type->type_name();
            } default:
                return false; // it fucking broke again
        }
    }

    bool ExpressionTypechecker::same_type(const std::string &type, const std::string &expected) const {
        return type == expected;
    }

    TypecheckerScope::TypecheckerScope(const std::shared_ptr<TypecheckerScope> &parent) : type_map({}), unused_variables({}), parent(parent) {}
    Error TypecheckerScope::declare(const std::shared_ptr<Statement> &stmt, const ExpressionTypechecker &expression_typechecker) {
        switch (stmt->type()) {
            case NodeType::NORMAL_VARIABLE_DECLARATION_STATEMENT: {
                const std::shared_ptr<NormalVariableDeclaration> normal_variable_declaration = std::static_pointer_cast<NormalVariableDeclaration>(stmt);
                if (this->type_map.count(normal_variable_declaration->var_name())) {
                    return Error(stmt->start(), stmt->end(), "Variable Error", "cannot declare variable '" + normal_variable_declaration->var_name() + "' as it already exists");
                }

                if (normal_variable_declaration->value() == nullptr) {
                    this->type_map[normal_variable_declaration->var_name()] = normal_variable_declaration->var_type()->string_representation();
                    this->unused_variables[normal_variable_declaration->var_name()] = normal_variable_declaration;
                    return Error(stmt->start(), stmt->end(), "", "");
                }

                const Result<std::string> value_type = expression_typechecker.get_type(normal_variable_declaration->value(), *this);
                if (!value_type.error().type().empty()) {
                    return value_type.error();
                }

                if (!expression_typechecker.same_type(value_type.result(), normal_variable_declaration->var_type())) {
                    return Error(normal_variable_declaration->value()->start(), normal_variable_declaration->value()->end(), "Type Error", "mismatched type, expected '" + normal_variable_declaration->var_type()->string_representation() + "', got '" + value_type.result() + "'");
                }

                this->type_map[normal_variable_declaration->var_name()] = normal_variable_declaration->var_type()->string_representation();
                this->unused_variables[normal_variable_declaration->var_name()] = normal_variable_declaration;
                return value_type.error();
            } case NodeType::VARIABLE_DECLARATION_WITH_EXPRESSION_CONDITION_STATEMENT: {
                const std::shared_ptr<VariableDeclarationWithExpressionCondition> variable_declaration_with_expression_condition = std::static_pointer_cast<VariableDeclarationWithExpressionCondition>(stmt);
                if (this->type_map.count(variable_declaration_with_expression_condition->var_name())) {
                    return Error(stmt->start(), stmt->end(), "Variable Error", "cannot declare variable '" + variable_declaration_with_expression_condition->var_name() + "' as it already exists");
                }

                this->type_map[variable_declaration_with_expression_condition->var_name()] = variable_declaration_with_expression_condition->var_type()->string_representation();
                this->unused_variables[variable_declaration_with_expression_condition->var_name()] = variable_declaration_with_expression_condition;
                const Result<std::string> condition_type = expression_typechecker.get_type(variable_declaration_with_expression_condition->condition(), *this);
                if (!condition_type.error().type().empty()) {
                    return condition_type.error();
                }

                if (!expression_typechecker.same_type(condition_type.result(), "bool")) {
                    return Error(variable_declaration_with_expression_condition->condition()->start(), variable_declaration_with_expression_condition->condition()->end(), "Type Error", "variable condition must be a 'bool', got '" + condition_type.result() + "'");
                }

                if (this->unused_variables.count(variable_declaration_with_expression_condition->var_name())) {
                    return Error(variable_declaration_with_expression_condition->condition()->start(), variable_declaration_with_expression_condition->condition()->end(), "Variable Error", "variable '" + variable_declaration_with_expression_condition->var_name() + "' was not used in the condition of the declaration");
                }

                if (variable_declaration_with_expression_condition->value() == nullptr) {
                    this->unused_variables[variable_declaration_with_expression_condition->var_name()] = variable_declaration_with_expression_condition;
                    return Error(stmt->start(), stmt->end(), "", "");
                }

                this->type_map.erase(variable_declaration_with_expression_condition->var_name());
                const Result<std::string> value_type = expression_typechecker.get_type(variable_declaration_with_expression_condition->value(), *this);
                if (!value_type.error().type().empty()) {
                    return value_type.error();
                }

                if (!expression_typechecker.same_type(value_type.result(), variable_declaration_with_expression_condition->var_type())) {
                    return Error(variable_declaration_with_expression_condition->value()->start(), variable_declaration_with_expression_condition->value()->end(), "Type Error", "mismatched type, expected '" + variable_declaration_with_expression_condition->var_type()->string_representation() + "', got '" + value_type.result() + "'");
                }

                this->type_map[variable_declaration_with_expression_condition->var_name()] = variable_declaration_with_expression_condition->var_type()->string_representation();
                this->unused_variables[variable_declaration_with_expression_condition->var_name()] = variable_declaration_with_expression_condition;
                return value_type.error();
            } default:
                return Error(stmt->start(), stmt->end(), "Typechecker Error", "not a declaration...");
        }
    }

    Error TypecheckerScope::assign(const std::shared_ptr<AssignmentStatement> &expr, const ExpressionTypechecker &expression_typechecker) {
        const std::shared_ptr<IdentifierExpression> identifier_expression = std::static_pointer_cast<IdentifierExpression>(expr->assigner());
        std::shared_ptr<Statement> unused_variable = nullptr;
        if (this->unused_variables.count(identifier_expression->symbol())) {
            unused_variable = this->unused_variables.at(identifier_expression->symbol());
        }

        if (this->type_map.count(identifier_expression->symbol())) {
            const Result<std::string> value_type = expression_typechecker.get_type(expr->assignee(), *this);
            if (!value_type.error().type().empty()) {
                return value_type.error();
            }
            
            if (!expression_typechecker.same_type(value_type.result(), this->type_map.at(identifier_expression->symbol()))) {
                return Error(expr->assignee()->start(), expr->assignee()->end(), "Type Error", "mismatched type, expected '" + this->type_map.at(identifier_expression->symbol()) + "', got '" + value_type.result() + "'");
            }
            
            if (unused_variable != nullptr) {
                if (!this->unused_variables.count(identifier_expression->symbol())) {
                    this->unused_variables[identifier_expression->symbol()] = unused_variable;
                }
            }

            return Error(expr->start(), expr->start(), "", "");
        }

        if (this->parent == nullptr) {
            return Error(expr->start(), expr->end(), "Variable Error", "cannot assign to variable '" + identifier_expression->symbol() + "' as it is not declared yet");
        }

        return this->parent->assign(expr, expression_typechecker);
    }

    Result<std::string> TypecheckerScope::get_value(const std::shared_ptr<IdentifierExpression> &expr) {
        if (this->type_map.count(expr->symbol())) {
            if (this->unused_variables.count(expr->symbol())) {
                this->unused_variables.erase(expr->symbol());
            }

            return Result<std::string>(this->type_map.at(expr->symbol()), Error(expr->start(), expr->start(), "", ""));
        }

        if (this->parent == nullptr) {
            return Result<std::string>("", Error(expr->start(), expr->end(), "Variable Error", "cannot retrieve the value of variable '" + expr->symbol() + "' as it is not declared yet"));
        }

        return this->parent->get_value(expr);
    }

    std::shared_ptr<Statement> TypecheckerScope::get_unused_variable() const {
        if (this->unused_variables.empty()) {
            return nullptr;
        }

        return this->unused_variables.begin()->second;
    }
    
    Typechecker::Typechecker(const std::shared_ptr<BlockStatement> &ast) : ast(ast) {}
    Error Typechecker::typecheck() {
        std::shared_ptr<TypecheckerScope> proband_scope = nullptr;
        return this->typecheck_(this->ast, proband_scope, ExpressionTypechecker(), false);
    }

    Error Typechecker::typecheck_(const std::shared_ptr<Statement> &stmt, std::shared_ptr<TypecheckerScope> &scope, const ExpressionTypechecker &expression_typechecker, const bool in_loop) {
        switch (stmt->type()) {
            case NodeType::BLOCK_STATEMENT: {
                const std::shared_ptr<BlockStatement> block = std::static_pointer_cast<BlockStatement>(stmt);
                std::shared_ptr<TypecheckerScope> child_scope = std::make_shared<TypecheckerScope>(scope);
                size_t body_size = block->body().size();
                for (size_t i = 0; i < body_size; i++) {
                    const std::shared_ptr<Statement> line = block->body()[i];
                    const Error error = this->typecheck_(line, child_scope, expression_typechecker, in_loop);
                    if (!error.type().empty()) {
                        return error;
                    }

                    if (line->type() == NodeType::BREAK_STATEMENT || line->type() == NodeType::CONTINUE_STATEMENT) {
                        if (i != body_size-1) {
                            return Error(block->body()[i+1]->start(), block->body()[body_size-1]->end(), "Control Flow Error", "unreachable code");
                        }

                        break;
                    }
                }

                const std::shared_ptr<Statement> unused_variable = child_scope->get_unused_variable();
                if (unused_variable != nullptr) {
                    std::string var_name;
                    switch (unused_variable->type()) {
                        case NodeType::NORMAL_VARIABLE_DECLARATION_STATEMENT:
                            var_name = std::static_pointer_cast<NormalVariableDeclaration>(unused_variable)->var_name();
                            break;
                        case NodeType::VARIABLE_DECLARATION_WITH_EXPRESSION_CONDITION_STATEMENT:
                            var_name = std::static_pointer_cast<VariableDeclarationWithExpressionCondition>(unused_variable)->var_name();
                            break;
                        default:
                            return Error(unused_variable->start(), unused_variable->end(), "Typechecker Error", "it broke again: '" + std::to_string(static_cast<int>(unused_variable->type())) + "'");
                    }

                    return Error(unused_variable->start(), unused_variable->end(), "Variable Error", "unused variable '" + var_name + "', if this variable declaration was not intended, go to file '" + unused_variable->start().file_name() + "' and find line " + std::to_string(unused_variable->start().line()) + " to remove the declaration");
                }

                return Error(block->start(), block->start(), "", "");
            } case NodeType::NORMAL_VARIABLE_DECLARATION_STATEMENT:
                return scope->declare(stmt, expression_typechecker);
            case NodeType::VARIABLE_DECLARATION_WITH_EXPRESSION_CONDITION_STATEMENT:
                return scope->declare(stmt, expression_typechecker);
            case NodeType::IF_ELSE_STATEMENT: {
                const std::shared_ptr<IfElseStatement> if_else = std::static_pointer_cast<IfElseStatement>(stmt);
                const Result<std::string> condition_type = expression_typechecker.get_type(if_else->condition(), *scope);
                if (!condition_type.error().type().empty()) {
                    return condition_type.error();
                }

                if (!expression_typechecker.same_type(condition_type.result(), "bool")) {
                    return Error(if_else->condition()->start(), if_else->condition()->end(), "Type Error", "expected a 'bool' for the condition, got '" + condition_type.result() + "'");
                }

                const Error if_body = this->typecheck_(if_else->body(), scope, expression_typechecker, in_loop);
                if (!if_body.type().empty()) {
                    return if_body;
                }

                if (if_else->else_body() == nullptr) {
                    return if_body;
                }

                return this->typecheck_(if_else->else_body(), scope, expression_typechecker, in_loop);
            } case NodeType::WHILE_STATEMENT: {
                const std::shared_ptr<WhileStatement> while_loop = std::static_pointer_cast<WhileStatement>(stmt);
                const Result<std::string> condition_type = expression_typechecker.get_type(while_loop->condition(), *scope);
                if (!condition_type.error().type().empty()) {
                    return condition_type.error();
                }

                if (!expression_typechecker.same_type(condition_type.result(), "bool")) {
                    return Error(while_loop->condition()->start(), while_loop->condition()->end(), "Type Error", "expected a 'bool' for the condition, got '" + condition_type.result() + "'");
                }

                return this->typecheck_(while_loop->body(), scope, expression_typechecker, true);
            } case NodeType::BREAK_STATEMENT:
                if (!in_loop) {
                    return Error(stmt->start(), stmt->end(), "Syntax Error", "'break' should not be outside of a loop");
                }

                return Error(stmt->start(), stmt->start(), "", "");
            case NodeType::CONTINUE_STATEMENT:
                if (!in_loop) {
                    return Error(stmt->start(), stmt->end(), "Syntax Error", "'continue' should not be outside of a loop");
                }

                return Error(stmt->start(), stmt->start(), "", "");
            case NodeType::FOREVER_STATEMENT: {
                const std::shared_ptr<ForeverStatement> forever_loop = std::static_pointer_cast<ForeverStatement>(stmt);
                return this->typecheck_(forever_loop->body(), scope, expression_typechecker, true);
            } case NodeType::ASSIGNMENT_STATEMENT: {
                const std::shared_ptr<AssignmentStatement> assignment = std::static_pointer_cast<AssignmentStatement>(stmt);
                switch (assignment->assigner()->type()) {
                    case NodeType::IDENTIFIER_EXPRESSION:
                        return scope->assign(assignment, expression_typechecker);
                    default:
                        return Error(assignment->start(), assignment->end(), "Typechecker Error", "i hate when it breaks '" + std::to_string(static_cast<int>(assignment->assigner()->type())) + "'");
                }
            } default: {
                const std::shared_ptr<Expression> expr = std::dynamic_pointer_cast<Expression>(stmt);
                if (expr) {
                    return expression_typechecker.get_type(expr, *scope).error();
                } else {
                    return Error(stmt->start(), stmt->end(), "Typechecker Error", "it broke: '" + std::to_string(static_cast<int>(stmt->type())) + "'");
                }
            }
        }
    }
}

