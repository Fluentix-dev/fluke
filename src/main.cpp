#include "errors/results.hpp"
#include "lexer/lexer.hpp"
#include "lexer/tokens.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Fluke is a language that programmers hate the most, even more than Brainfuck...\nType 'fluke help' to show all the commands\n";
        return 0;
    }

    std::string command = argv[1];
    if (command == "help") {
        std::cout << "List of commands:\n[1] fluke --version: Shows the version of fluke that you are using\n[2] fluke help: Shows all the commands you can use\n[3] fluke clean: Clears the terminal\n[4] fluke conv <file_name>: Transpiles the code to C++";
        return 0;
    }

    if (command == "--version") {
        std::cout << "Fluke version Alpha - In development...\n";
        return 0;
    }

    if (command == "clean") {
        #ifdef _WIN32
            system("cls");
        #else
            std::cout << "\033[2J\033[H" << std::flush;
        #endif
        return 0;
    }

    if (command == "conv") {
        if (argc == 2) {
            std::cout << "File name required\n";
            return 0;
        }

        std::string file_name = argv[2];
        std::ifstream src(file_name);
        if (!src) {
            std::cout << "File '" << file_name << "' cannot be found\n";
            return 0;
        }

        std::stringstream code;
        std::string line;
        while (src.is_open() && std::getline(src, line)) {
            code << line << "\n";
        }

        std::string real = code.str();
        if (!real.empty()) {
            real.erase(std::prev(real.end()));
        }

        fluke::Lexer lexer = fluke::Lexer(file_name, real);
        const fluke::Result<std::vector<fluke::Token>> tokens = lexer.tokenize();
        if (!tokens.error().type().empty()) {
            tokens.error().display_error();
            return 1;
        }

        for (fluke::Token &token : tokens.result()) {
            std::cout << static_cast<int>(token.type()) << " " << token.value() << "\n";
        }

        return 0;
    }

    std::cout << "Command '" << command << "' not found\n";
    return 0;
}
