#include "results.hpp"
#include "errors.hpp"
#include "../lexer/tokens.hpp"
#include "../parser/nodes.hpp"
#include "../parser/statements.hpp"
#include <vector>
#include <memory>
namespace fluke {
    template<typename T>
    Result<T>::Result(const T &result, const Error &error) : result_(result), error_(error) {}

    template<typename T>
    T Result<T>::result() const {
        return this->result_;
    }

    template<typename T>
    Error Result<T>::error() const {
        return this->error_;
    }

    template struct Result<std::vector<Token>>;
    template struct Result<std::shared_ptr<Statement>>;
    template struct Result<std::shared_ptr<Expression>>;
    template struct Result<std::shared_ptr<BlockStatement>>;
}
