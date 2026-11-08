#include "positions.hpp"
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>
namespace fluke {
    Position::Position(const std::string &file_name, const std::vector<std::string> &lines, const size_t line, const size_t column) : file_name_(file_name), line_(line), column_(column), lines_(lines) {}

    std::string Position::file_name() const {
        return this->file_name_;
    }
    
    size_t Position::line() const {
        return this->line_;
    }

    size_t Position::column() const {
        return this->column_;
    }

    std::vector<std::string> Position::lines() const {
        return this->lines_;
    }

    void Position::advance(const char c) {
        if (c == '\n') {
            this->column_ = 1;
            this->line_++;
        } else {
            this->column_++;
        }
    }
}
