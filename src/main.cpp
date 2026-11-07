#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Fluke is a language that programmers hate the most, even more than Brainfuck...\nType 'fluke help' to show all the commands\n";
        return 0;
    }

    std::string command = argv[1];
    if (command == "help") {
        std::cout << "List of commands:\n[1] fluke --version: Shows the version of fluke that you are using\n[2] fluke help: Shows all the commands you can use\n[3] fluke clean Clears the terminal\n";
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

    std::cout << "Command '" << command << "' not found\n";
    return 0;
}
