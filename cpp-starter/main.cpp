#include <iostream>
#include <string>
#include <vector>

#include "cxxopts.hpp"

// Itt olvassuk be a saját feladatainkat:
#include "problems/dfa.h"
#include "problems/det.h"   // <--- Ezt most adtuk hozzá

int runProblem(int argc, char* argv[]) {

    std::vector<Problem *> problems;

    // Mindkét feladatunk regisztrálva van:
    problems.push_back(new DFAProblem());
    problems.push_back(new DetProblem());   // <--- És ide is bekerült

    cxxopts::Options options("project", "Run the specific problem");

    options.add_options()
        ("i,input", "Input file name", cxxopts::value<std::string>())
        ("o,output", "Output file name", cxxopts::value<std::string>())
        ("h,help", "Print usage");

    for (Problem *p : problems) {
        p->initialize_parser(options);
    }

    cxxopts::ParseResult args = options.parse(argc, argv);

    for (Problem *p : problems) {
        if (p->is_chosen_problem(args)) {
            p->run(args);
            return 0;
        }
    }

    std::cout << options.help() << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {

    try {
        return runProblem(argc, argv);
    } catch (const cxxopts::exceptions::exception &e) {
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        return 1;
    }
}