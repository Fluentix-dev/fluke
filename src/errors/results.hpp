#pragma once
#include "errors.hpp"
namespace fluke {
    template<typename T>
    struct Result {
    public:
        Result(const T &result, const Error &error);
        T result() const;
        Error error() const;
    private:
        T result_;
        Error error_;
    };
}