#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Lexer.h"
#include "Parser.h"

void printUsage() {
    std::cout << "Usage:\n"
              << "Run a script:\n"
              << "\tlegba script\n"
              << "Start REPL:\n"
              << "\tlegba {--repl|-r}" << std::endl;
}

void printVersion() {
    std::cout << "Legba 1.0.0\nCopyright (c) Samuel Thesing\n" << std::endl;
}

void runRepl() {
    std::cout << "Starting REPL" << std::endl;

    std::cout << "Exiting REPL" << std::endl;
}

void runScript(const std::string& filename) {
    std::ifstream file;
    file.open(filename, std::ios::in);
    if (!file.is_open()) {
        file.close();
        std::cerr << "Failed to open file '" << filename << "'" << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    std::string source(size, ' ');
    file.seekg(0, std::ios::beg);
    file.read(&source[0], size);
    file.close();

    std::cout << "Parsing script '" << filename << "'" << std::endl;
    auto lexer = Lexer();
    std::vector<Token> tokens = lexer.lex(source);
    /*for (auto const& token : tokens) {
        std::cout << token << std::endl;
    }*/

    auto parser = Parser();
    if (!parser.parse(tokens)) {
        return;
    }
    parser.printEnv();

    std::cout << "Running script" << std::endl;

    std::cout << "Finished running" << std::endl;
}

int main(int argc, char** argv) {
    std::vector<std::string> args = std::vector<std::string>();
    for (int i = 1; i < argc; i++) {
        args.emplace_back(argv[i]);
    }

    printVersion();

    if (args.empty() || args[0] == "--help" || args[0] == "-h" || args[0] == "?") {
        printUsage();
    } else if (args[0] == "--repl" || args[0] == "-r") {
        runRepl();
    } else if (args.size() == 1) {
        runScript(args[0]);
    } else {
        printUsage();
    }


	return 0;
}