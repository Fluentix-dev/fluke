#include "errors.hpp"
#include "../positions/positions.hpp"
#include <cstddef>
#include <iostream>
#include <string>
namespace fluke {
    Error::Error(const Position &start, const Position &end, const std::string &type, const std::string &details) : start_(start), end_(end), type_(type), details_(details) {}
    Position Error::start() const {
        return this->start_;
    }

    Position Error::end() const {
        return this->end_;
    }

    std::string Error::type() const {
        return this->type_;
    }

    std::string Error::details() const {
        return this->details_;
    }

    void Error::display_error() const {
        const auto count_digits = [](size_t n) -> size_t{
            if (n == 0) {
                return 1;
            }

            size_t cnt = 0;
            while (n != 0) {
                cnt++;
                n /= 10;
            }

            return cnt;
        };

        std::cout << "Transpiling \033[1m\033[31mstopped\033[0m because of the following \033[1m\033[31merror\033[0m:\nFile \033[1m'" << this->start_.file_name() << "'\033[0m, line \033[1m" << this->start_.line() << "\033[0m, column \033[1m" << this->start_.column() << "\033[0m:\n";
        const size_t start_col = this->start_.column();
        const size_t start_line = this->start_.line();
        const size_t start_digits = count_digits(start_line);
        const size_t end_col = this->end_.column();
        const size_t end_line = this->end_.line();
        const size_t end_digits = count_digits(end_line);
        const std::vector<std::string> lines = this->start_.lines();
        const std::string line_start = lines[start_line-1];
        const std::string line_end = lines[end_line-1];
        if (start_line == end_line) {
            std::cout << "\033[1m\033[32m" << start_line << " || \033[0m";
            for (size_t i = 0; i < line_start.size(); i++) {
                if (i == start_col-1) {
                    std::cout << "\033[1m";
                }

                if (i == end_col-1) {
                    std::cout << "\033[0m";
                }

                std::cout << line_start[i];
            }

            std::cout << "\n";
            for (size_t i = 0; i < start_digits+4; i++) {
                std::cout << " ";
            }

            for (size_t i = 1; i < start_col; i++) {
                std::cout << " ";
            }

            std::cout << "\033[1m\033[31m";
            for (size_t i = start_col; i < end_col; i++) {
                std::cout << "^";
            }
        } else {
            size_t difference = end_digits - start_digits;
            for (size_t i = 0; i < difference; i++) {
                std::cout << " ";
            }

            std::cout << "\033[1m\033[32m" << start_line << " || \033[0m";
            for (size_t i = 0; i < line_start.size(); i++) {
                if (i == start_col-1) {
                    std::cout << "\033[1m";
                }

                std::cout << line_start[i];
            }

            std::cout << "\033[0m\n";
            for (size_t i = 0; i < end_digits+4; i++) {
                std::cout << " ";
            }

            for (size_t i = 1; i < start_col; i++) {
                std::cout << " ";
            }

            std::cout << "\033[1m\033[31m";
            for (size_t i = start_col; i < line_start.size(); i++) {
                std::cout << "^";
            }

            std::cout << "\033[0m\n";
            for (size_t i = start_line+1; i < end_line; i++) {
                difference = end_digits - count_digits(i);
                for (size_t i = 0; i < difference; i++) {
                    std::cout << " ";
                }

                std::cout << "\033[1m\033[32m" << i << " || \033[1m" << lines[i-1] << "\033[0m\n";
                for (size_t i = 0; i < end_digits+4; i++) {
                    std::cout << " ";
                }

                std::cout << "\033[1m\033[31m";
                for (const char &_ : lines[i-1]) {
                    std::cout << "^";
                }

                std::cout << "\033[0m\n";
            }

            std::cout << "\033[1m\033[32m" << end_line << " || \033[1m";
            for (size_t i = 0; i < line_end.size(); i++) {
                if (i < end_col-1) {
                    std::cout << "\033[0m";
                }

                std::cout << line_end[i];
            }

            std::cout << "\033[0m\n";
            for (size_t i = 0; i < end_digits+4; i++) {
                std::cout << " ";
            }

            std::cout << "\033[1m\033[31m";
            for (size_t i = 1; i < end_col; i++) {
                std::cout << "^";
            }
        }

        std::cout << "\033[0m\n\033[1m\033[31m" << this->type_ << "\033[0m: " << this->details_ << "\n";
    }
}
