#pragma once
#include "../positions/positions.hpp"
#include <string>
// we have this file because later on we might do type unions or such
namespace fluke {
    enum struct TypeType {
        IDENTIFIER
    };

    struct Type {
    public:
        Type(const Position &start, const Position &end, const TypeType type);
        Position start() const;
        Position end() const;
        TypeType type() const;
    private:
        Position start_;
        Position end_;
        TypeType type_;
    };

    struct IdentifierType : public Type {
    public:
        IdentifierType(const Position &start, const Position &end, const std::string &type_name);
        std::string type_name() const;
    private:
        std::string type_name_;
    };
}