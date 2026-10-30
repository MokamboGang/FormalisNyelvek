#ifndef MINI_H
#define MINI_H

#include "../problem.hpp"
#include "../cxxopts.hpp"

class MiniProblem : public Problem {
public:
    void initialize_parser(cxxopts::Options& options) override;
    bool is_chosen_problem(const cxxopts::ParseResult& args) override;
    int run(const cxxopts::ParseResult& args) override;
};

#endif