#ifndef DFA_H
#define DFA_H

#include "../problem.hpp"
#include "../cxxopts.hpp"
#include <string>

struct Transition {
    std::string from;
    char symbol;
    std::string to;
};

class DFAProblem : public Problem {
public:
    void initialize_parser(cxxopts::Options& options) override;
    bool is_chosen_problem(const cxxopts::ParseResult& args) override;
    int run(const cxxopts::ParseResult& args) override;
};

#endif