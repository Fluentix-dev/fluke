#include "nodes.hpp"
#include "../positions/positions.hpp"
namespace fluke {
    Statement::Statement(const Position &start, const Position &end, const NodeType type) : start_(start), end_(end), type_(type) {}
    Position Statement::start() const {
        return this->start_;
    }

    Position Statement::end() const {
        return this->end_;  
    }

    NodeType Statement::type() const {
        return this->type_;
    }

    Expression::Expression(const Position &start, const Position &end, const NodeType type) : Statement(start, end, type) {}
}
