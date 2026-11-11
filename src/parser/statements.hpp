#pragma once
#include "nodes.hpp"
#include <vector>
#include <memory>
namespace fluke {
    struct BlockStatement : public Statement {
    public:
        BlockStatement(const Position &start, const Position &end, const std::vector<std::shared_ptr<Statement>> &body);
        std::vector<std::shared_ptr<Statement>> body() const;
    private:
        std::vector<std::shared_ptr<Statement>> body_;
    };
}