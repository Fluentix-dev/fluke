#include "errors/results.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokens.hpp"
#include "parser/parser.hpp"
#include "parser/statements.hpp"
#include "parser/expressions.hpp"
#include "parser/types.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

void debug_type(const std::shared_ptr<fluke::Type> &type) {
    switch (type->type()) {
        case fluke::TypeType::IDENTIFIER: {
            std::shared_ptr<fluke::IdentifierType> identifier_type = std::static_pointer_cast<fluke::IdentifierType>(type);
            std::cout << identifier_type->type_name();
            break;
        } default:
            std::cout << "fuck you it broke again hahahahaha\n";
    }
}

void debug_expr(const std::shared_ptr<fluke::Expression> &expr) {
    if (expr == nullptr) {
        std::cout << "NULLPTR";
        return;
    }

    switch (expr->type()) {
        case fluke::NodeType::BINARY_EXPRESSION: {
            std::shared_ptr<fluke::BinaryExpression> binary_expression = std::static_pointer_cast<fluke::BinaryExpression>(expr);
            std::cout << "(";
            debug_expr(binary_expression->lhs());
            std::cout << " " << binary_expression->op() << " ";
            debug_expr(binary_expression->rhs());
            std::cout << ")";
            break;
        } case fluke::NodeType::UNARY_EXPRESSION: {
            std::shared_ptr<fluke::UnaryExpression> unary_expression = std::static_pointer_cast<fluke::UnaryExpression>(expr);
            std::cout << "(" << unary_expression->op();
            debug_expr(unary_expression->value());
            std::cout << ")";
            break;
        } case fluke::NodeType::INT_EXPRESSION: {
            std::shared_ptr<fluke::IntExpression> int_expression = std::static_pointer_cast<fluke::IntExpression>(expr);
            std::cout << int_expression->value();
            break;
        } case fluke::NodeType::FLOAT_EXPRESSION: {
            std::shared_ptr<fluke::FloatExpression> float_expression = std::static_pointer_cast<fluke::FloatExpression>(expr);
            std::cout << float_expression->value();
            break;
        } case fluke::NodeType::IDENTIFIER_EXPRESSION: {
            std::shared_ptr<fluke::IdentifierExpression> identifier_expression = std::static_pointer_cast<fluke::IdentifierExpression>(expr);
            std::cout << identifier_expression->symbol();
            break;
        } case fluke::NodeType::GROUPING_EXPRESSION: {
            std::shared_ptr<fluke::GroupExpression> group_expression = std::static_pointer_cast<fluke::GroupExpression>(expr);
            debug_expr(group_expression->value());
            break;
        } default:
            std::cout << "fuck you it broke hahahahhaha\n";
            break;
    }
}

void debug_stmt(const std::shared_ptr<fluke::Statement> &stmt, const size_t indentation = 0) {
    for (size_t i = 0; i < indentation; i++) {
        std::cout << "    ";
    }

    switch (stmt->type()) {
        case fluke::NodeType::BLOCK_STATEMENT: {
            std::shared_ptr<fluke::BlockStatement> block_statement = std::static_pointer_cast<fluke::BlockStatement>(stmt);
            std::cout << "(BLOCK STATEMENT, len=" << block_statement->body().size() << ") {\n";
            for (const std::shared_ptr<fluke::Statement> &line : block_statement->body()) {
                debug_stmt(line, indentation+1);
                std::cout << "\n";
            }

            std::cout << "}";
            break;
        } case fluke::NodeType::NORMAL_VARIABLE_DECLARATION_STATEMENT: {
            std::shared_ptr<fluke::NormalVariableDeclaration> normal_variable_declaration = std::static_pointer_cast<fluke::NormalVariableDeclaration>(stmt);
            std::cout << "LET " << normal_variable_declaration->var_name() << ": ";
            debug_type(normal_variable_declaration->var_type());
            std::cout << " = ";
            debug_expr(normal_variable_declaration->value());
            break;
        } case fluke::NodeType::VARIABLE_DECLARATION_WITH_EXPRESSION_CONDITION_STATEMENT: {
            std::shared_ptr<fluke::VariableDeclarationWithExpressionCondition> variable_declaration_with_expression_condition = std::static_pointer_cast<fluke::VariableDeclarationWithExpressionCondition>(stmt);
            std::cout << "LET " << variable_declaration_with_expression_condition->var_name() << ": ";
            debug_type(variable_declaration_with_expression_condition->var_type());
            std::cout << "(";
            debug_expr(variable_declaration_with_expression_condition->condition());
            std::cout << ") = ";
            debug_expr(variable_declaration_with_expression_condition->value());
            break;
        } default:
            debug_expr(std::static_pointer_cast<fluke::Expression>(stmt));
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Fluke is a language that programmers hate the most, even more than Brainfuck...\nType 'fluke help' to show all the commands\n";
        return 0;
    }

    std::string command = argv[1];
    if (command == "help") {
        std::cout << "List of commands:\n[1] fluke --version: Shows the version of fluke that you are using\n[2] fluke help: Shows all the commands you can use\n[3] fluke clean: Clears the terminal\n[4] fluke conv <file_name>: Transpiles the code to C++";
        return 0;
    }

    if (command == "--version") {
        std::cout << "Fluke version Alpha - In development...\n";
        return 0;
    }

    if (command == "clean") {
        #ifdef _WIN32
            system("cls");
        #else
            std::cout << "\033[2J\033[H" << std::flush;
        #endif
        return 0;
    }

    if (command == "conv") {
        if (argc == 2) {
            std::cout << "File name required\n";
            return 0;
        }

        std::string file_name = argv[2];
        std::ifstream src(file_name);
        if (!src) {
            std::cout << "File '" << file_name << "' cannot be found\n";
            return 0;
        }

        std::stringstream code;
        std::string line;
        while (src.is_open() && std::getline(src, line)) {
            code << line << "\n";
        }

        std::string real = code.str();
        if (!real.empty()) {
            real.erase(std::prev(real.end()));
        }

        fluke::Lexer lexer = fluke::Lexer(file_name, real);
        const fluke::Result<std::vector<fluke::Token>> tokens = lexer.tokenize();
        if (!tokens.error().type().empty()) {
            tokens.error().display_error();
            return 1;
        }

        fluke::Parser parser = fluke::Parser(tokens.result());
        const fluke::Result<std::shared_ptr<fluke::BlockStatement>> ast = parser.generate_ast();
        if (!ast.error().type().empty()) {
            ast.error().display_error();
            return 1;
        }

        debug_stmt(ast.result(), 0ULL);
        std::cout << "\nTranspiling '\033[1m" << file_name << "\033[0m' to C++ code is a \033[1m\033[32msuccess\033[0m because your code is better than \033[1m\033[32m99.999%\033[0m of Fluke coders in the \033[1mwhole world\033[0m!\n";
        return 0;
    }

    std::cout << "Command '" << command << "' not found\n";
    return 0;
}
