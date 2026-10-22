#include "dfa.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void DFAProblem::initialize_parser(cxxopts::Options& options) {
    options.add_options()("c,check", "Szavak ellenorzese", cxxopts::value<string>());
}

bool DFAProblem::is_chosen_problem(const cxxopts::ParseResult& args) {
    return args.count("check") > 0;
}

int DFAProblem::run(const cxxopts::ParseResult& args) {
    string input_file = args["input"].as<string>();
    string output_file = args["output"].as<string>();
    string check_words = args["check"].as<string>();

    ifstream file(input_file);
    if (!file.is_open()) return 1;

    vector<string> states, alphabet, final_states;
    vector<Transition> transitions;
    string start_state, line, word;

    getline(file, line); { stringstream ss(line); while (ss >> word) states.push_back(word); }
    getline(file, line); { stringstream ss(line); while (ss >> word) alphabet.push_back(word); }
    getline(file, line); { stringstream ss(line); ss >> start_state; }
    getline(file, line); { stringstream ss(line); while (ss >> word) final_states.push_back(word); }

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line); Transition t;
        ss >> t.from >> t.symbol >> t.to;
        transitions.push_back(t);
    }
    file.close();

    vector<string> words_to_test;
    stringstream ss_words(check_words);
    string current_word;
    while (getline(ss_words, current_word, ',')) words_to_test.push_back(current_word);

    ofstream out(output_file);
    for (const string& w : words_to_test) {
        string current_state = start_state;
        bool is_accepted = true;

        for (char letter : w) {
            bool found_rule = false;
            for (const auto& t : transitions) {
                if (t.from == current_state && t.symbol == letter) {
                    current_state = t.to;
                    found_rule = true;
                    break;
                }
            }
            if (!found_rule) { is_accepted = false; break; }
        }

        if (is_accepted) {
            bool is_final = false;
            for (const string& f : final_states) {
                if (current_state == f) { is_final = true; break; }
            }
            is_accepted = is_final;
        }

        out << (is_accepted ? "IGEN\n" : "NEM\n");
    }
    out.close();
    return 0;
}