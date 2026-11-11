#include "statements.hpp"
#include "nodes.hpp"
#include <vector>
#include <memory>
namespace fluke {
    BlockStatement::BlockStatement(const Position &start, const Position &end, const std::vector<std::shared_ptr<Statement>> &body) : Statement(start, end, NodeType::BLOCK_STATEMENT), body_(body) {}

    std::vector<std::shared_ptr<Statement>> BlockStatement::body() const {
        return this->body_;
    }
}