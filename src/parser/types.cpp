#include "types.hpp"
#include "../positions/positions.hpp"
#include <string>
namespace fluke {
    Type::Type(const Position &start, const Position &end, const TypeType type) : start_(start), end_(end), type_(type) {}
    Position Type::start() const {
        return this->start_;
    }

    Position Type::end() const {
        return this->end_;
    }

    TypeType Type::type() const {
        return this->type_;
    }

    IdentifierType::IdentifierType(const Position &start, const Position &end, const std::string &type_name) : Type(start, end, TypeType::IDENTIFIER), type_name_(type_name) {}
    std::string IdentifierType::type_name() const {
        return this->type_name_;
    }
}