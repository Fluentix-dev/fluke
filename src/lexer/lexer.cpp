#include "lexer.hpp"
#include "tokens.hpp"
#include "../errors/errors.hpp"
#include "../errors/results.hpp"
#include <string>
#include <vector>
namespace fluke {
    Lexer::Lexer(const std::string &file_name, const std::string &src) : src(src), index(0), position("", {}, 0, 0) {
        std::vector<std::string> lines = {};
        std::string line = "";
        for (const char &c : src + '\n') {
            if (c == '\n') {
                lines.push_back(line);
                line = "";
            } else {
                line += c;
            }
        }

        this->position = Position(file_name, lines, 1, 1);
    }

    void Lexer::advance() {
        this->position.advance(this->src[this->index]);
        this->index++;
    }

    Result<std::vector<Token>> Lexer::tokenize() {
        std::vector<Token> tokens = {};
        while (this->index < this->src.size()) {
            switch (this->src[this->index]) {
                case ' ':
                    this->advance();
                    break;
                case '\n':
                    this->advance();
                    break;
                case '\t':
                    this->advance();
                    break;
                case '\r':
                    this->advance();
                    break;
                case '+': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::PLUS, "+"));
                    break;
                } case '-': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::MINUS, "-"));
                    break;
                } case '*': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::MULTIPLY, "*"));
                    break;
                } case '/': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::DIVIDE, "/"));
                    break;
                } case '(': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::LEFT_PAREN, "("));
                    break;
                } case ')': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::RIGHT_PAREN, ")"));
                    break;
                } case ';': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::SEMICOLON, ";"));
                    break;
                } case '=': {
                    Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::EQUALS, "="));
                    break;
                } default:
                    if (this->src[this->index] == '.' || ('0' <= this->src[this->index] && this->src[this->index] <= '9')) {
                        Position start = this->position;
                        std::string number = "";
                        bool dot = false;
                        while (this->index < this->src.size() && (this->src[this->index] == '.' || ('0' <= this->src[this->index] && this->src[this->index] <= '9'))) {
                            if (this->src[this->index] == '.') {
                                if (dot) {
                                    start = this->position;
                                    this->advance();
                                    return Result<std::vector<Token>>(tokens, Error(start, this->position, "Syntax Error", "extra '.' in number literals"));
                                }

                                dot = true;
                            }

                            number += this->src[this->index];
                            this->advance();
                        }

                        if (dot) {
                            tokens.push_back(Token(start, this->position, TokenType::FLOAT, number));
                        } else {
                            tokens.push_back(Token(start, this->position, TokenType::INT, number));
                        }

                        continue;
                    }

                    if (this->src[this->index] == '_' || ('A' <= this->src[this->index] && this->src[this->index] <= 'Z') || ('a' <= this->src[this->index] && this->src[this->index] <= 'z')) {
                        Position start = this->position;
                        std::string identifier = "";
                        while (this->index < this->src.size() && (this->src[this->index] == '_' || ('A' <= this->src[this->index] && this->src[this->index] <= 'Z') || ('a' <= this->src[this->index] && this->src[this->index] <= 'z'))) {
                            identifier += this->src[this->index];
                            this->advance();
                        }

                        tokens.push_back(Token(start, this->position, TokenType::IDENTIFIER, identifier));
                        continue;
                    }

                    const Position start = this->position;
                    const char error_char = this->src[this->index];
                    this->advance();
                    return Result<std::vector<Token>>(tokens, Error(start, this->position, "Syntax Error", std::string("unexpected character: '") + error_char + "'"));
            }
        }

        return Result<std::vector<Token>>(tokens, Error(this->position, this->position, "", ""));
    }
}
