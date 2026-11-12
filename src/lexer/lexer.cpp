#include "lexer.hpp"
#include "tokens.hpp"
#include "../errors/errors.hpp"
#include "../errors/results.hpp"
#include <string>
#include <vector>
#include <unordered_map>
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
        const std::unordered_map<std::string, TokenType> KEYWORDS = {
            {"let", TokenType::LET}
        };

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
                case '#':
                    while (this->index < this->src.size() && this->src[this->index] != '\n') {
                        this->advance();
                    }

                    break;
                case '+': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::PLUS, "+"));
                    break;
                } case '-': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::MINUS, "-"));
                    break;
                } case '*': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::MULTIPLY, "*"));
                    break;
                } case '/': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::DIVIDE, "/"));
                    break;
                } case '(': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::LEFT_PAREN, "("));
                    break;
                } case ')': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::RIGHT_PAREN, ")"));
                    break;
                } case ';': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::SEMICOLON, ";"));
                    break;
                } case '=': {
                    const Position start = this->position;
                    this->advance();
                    if (this->index >= this->src.size()) {
                        tokens.push_back(Token(start, this->position, TokenType::EQUALS, "="));
                        continue;
                    }

                    if (this->src[this->index] == '=') {
                        this->advance();
                        tokens.push_back(Token(start, this->position, TokenType::DOUBLE_EQUALS, "=="));
                    } else {
                        tokens.push_back(Token(start, this->position, TokenType::EQUALS, "="));
                    }

                    break;
                } case '!': {
                    Position start = this->position;
                    this->advance();
                    if (this->index >= this->src.size()) {
                        start = this->position;
                        this->advance();
                        return Result<std::vector<Token>>(tokens, Error(start, this->position, "Syntax Error", "expected '='"));
                    }

                    if (this->src[this->index] == '=') {
                        this->advance();
                        tokens.push_back(Token(start, this->position, TokenType::NOT_EQUALS, "!="));
                    } else {
                        start = this->position;
                        this->advance();
                        return Result<std::vector<Token>>(tokens, Error(start, this->position, "Syntax Error", "expected '=', got '" + this->src[this->index]));
                    }

                    break;
                } case '>': {
                    const Position start = this->position;
                    this->advance();
                    if (this->index >= this->src.size()) {
                        tokens.push_back(Token(start, this->position, TokenType::GREATER_THAN, ">"));
                        continue;
                    }

                    if (this->src[this->index] == '=') {
                        this->advance();
                        tokens.push_back(Token(start, this->position, TokenType::GREATER_THAN_OR_EQUALS, ">="));
                    } else {
                        tokens.push_back(Token(start, this->position, TokenType::GREATER_THAN, ">"));
                    }

                    break;
                } case '<': {
                    const Position start = this->position;
                    this->advance();
                    if (this->index >= this->src.size()) {
                        tokens.push_back(Token(start, this->position, TokenType::SMALLER_THAN, "<"));
                        continue;
                    }

                    if (this->src[this->index] == '=') {
                        this->advance();
                        tokens.push_back(Token(start, this->position, TokenType::SMALLER_THAN_OR_EQUALS, "<="));
                    } else {
                        tokens.push_back(Token(start, this->position, TokenType::SMALLER_THAN, "<"));
                    }

                    break;
                } case ':': {
                    const Position start = this->position;
                    this->advance();
                    tokens.push_back(Token(start, this->position, TokenType::COLON, ":"));
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
                        const Position start = this->position;
                        std::string identifier = "";
                        while (this->index < this->src.size() && (this->src[this->index] == '_' || ('A' <= this->src[this->index] && this->src[this->index] <= 'Z') || ('a' <= this->src[this->index] && this->src[this->index] <= 'z') || ('0' <= this->src[this->index] && this->src[this->index] <= '9'))) {
                            identifier += this->src[this->index];
                            this->advance();
                        }

                        if (KEYWORDS.count(identifier)) {
                            tokens.push_back(Token(start, this->position, KEYWORDS.at(identifier), identifier));
                        } else {
                            tokens.push_back(Token(start, this->position, TokenType::IDENTIFIER, identifier));
                        }

                        continue;
                    }

                    const Position start = this->position;
                    const char error_char = this->src[this->index];
                    this->advance();
                    return Result<std::vector<Token>>(tokens, Error(start, this->position, "Syntax Error", std::string("unexpected character: '") + error_char + "'"));
            }
        }

        Position start = this->position;
        this->advance();
        tokens.push_back(Token(start, this->position, TokenType::END_OF_FILE, "EOF"));
        return Result<std::vector<Token>>(tokens, Error(this->position, this->position, "", ""));
    }
}