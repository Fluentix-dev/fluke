#pragma once
#include <cstddef>
#include <string>
#include <vector>
namespace fluke {
    struct Position {
    public:
        Position(const std::string &file_name, const std::vector<std::string> &lines, const size_t line, const size_t column);
        std::string file_name() const;
        size_t line() const;
        size_t column() const;
        std::vector<std::string> lines() const;
        void advance(const char c);
    private:
        std::string file_name_;
        size_t line_;
        size_t column_;
        std::vector<std::string> lines_;
    };
}
