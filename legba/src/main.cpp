#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <stack>

#include "Lexer.h"
#include "Parser.h"

std::string durationAsString(std::chrono::time_point<std::chrono::high_resolution_clock> start, std::chrono::time_point<std::chrono::high_resolution_clock> end) {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    auto times = std::stack<int>();
    times.emplace((int)(duration % 1000)); // us
    duration /= 1000;
    times.emplace((int)(duration % 1000)); // ms
    duration /= 1000;
    times.emplace((int)(duration % 60)); // s
    duration /= 60;
    times.emplace((int)(duration % 60)); // min
    duration /= 60;
    times.emplace((int)duration); // h

    while (!times.empty() && times.top() == 0) times.pop();

    if (times.empty()) {
        return "0us";
    }

    std::string timeNames[] = {"us", "ms", "s", "min", "h"};
    std::string result;
    while (!times.empty()) {
        result += std::to_string(times.top()) + timeNames[times.size()-1] + ' ';
        times.pop();
    }
    result.pop_back();
    return result;
}

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
        std::cout << "Failed to open file '" << filename << "'" << std::endl;
        return;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    auto timeStart = std::chrono::high_resolution_clock::now();

    std::cout << "-- Parsing script '" << filename << "'" << std::endl;
    auto lexer = Lexer();
    std::vector<Token> tokens = lexer.lex(source);
    /*for (auto const& token : tokens) {
        std::cout << token << std::endl;
    }*/

    auto parser = Parser();
    if (!parser.parse(tokens)) {
        std::cout << "-- Failed to parse script ... Exiting" << std::endl;
        return;
    }

    auto timeEnd = std::chrono::high_resolution_clock::now();

    std::cout << "-- Compilation took " << durationAsString(timeStart, timeEnd) << std::endl;

    parser.printEnv();

    std::cout << "-- Running script" << std::endl;

    std::cout << "-- Finished running" << std::endl;


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