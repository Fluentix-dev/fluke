#pragma once
#include "../positions/positions.hpp"
#include <string>
namespace fluke {
    struct Error {
    public:
        Error(const Position &start, const Position &end, const std::string &type, const std::string &details);
        Position start() const;
        Position end() const;
        std::string type() const;
        std::string details() const;
        void display_error() const;
    private:
        Position start_;
        Position end_;
        std::string type_;
        std::string details_;
    };
}
