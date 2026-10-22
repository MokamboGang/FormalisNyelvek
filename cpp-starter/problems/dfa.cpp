#include "dfa.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void DFAProblem::initialize_parser(cxxopts::Options& options) {
    options.add_options()("c,check", "Szavak ellenorzese", cxxopts::value<std::string>());
}

bool DFAProblem::is_chosen_problem(const cxxopts::ParseResult& args) {
    return args.count("check") > 0;
}

int DFAProblem::run(const cxxopts::ParseResult& args) {
    cout << "SIKERES INDULAS: 1. feladat (Szoellenorzes) fut..." << endl;
    string input_file = args["input"].as<std::string>();
    string check_words = args["check"].as<std::string>();

    ifstream file(input_file);
    if (!file.is_open()) { cout << "Hiba: Nem tudom megnyitni a bemeneti fajlt!" << endl; return 1; }

    vector<string> states; vector<string> alphabet; string start_state; vector<string> final_states;
    vector<Transition> transitions; string line, word;

    getline(file, line); stringstream ss1(line); while (ss1 >> word) states.push_back(word);
    getline(file, line); stringstream ss2(line); while (ss2 >> word) alphabet.push_back(word);
    getline(file, line); stringstream ss3(line); ss3 >> start_state;
    getline(file, line); stringstream ss4(line); while (ss4 >> word) final_states.push_back(word);

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line); Transition t;
        ss >> t.from >> t.symbol >> t.to;
        transitions.push_back(t);
    }
    file.close();

    vector<string> words_to_test; stringstream ss_words(check_words); string current_word;
    while (getline(ss_words, current_word, ',')) words_to_test.push_back(current_word);

    cout << "\n=== 1. FELADAT (KIMENET) ===" << endl;
    for (int i = 0; i < words_to_test.size(); i++) {
        string w = words_to_test[i]; string current_state = start_state; bool is_accepted = true;
        for (int j = 0; j < w.length(); j++) {
            char letter = w[j]; bool found_rule = false;
            for (int k = 0; k < transitions.size(); k++) {
                if (transitions[k].from == current_state && transitions[k].symbol == letter) {
                    current_state = transitions[k].to; found_rule = true; break;
                }
            }
            if (!found_rule) { is_accepted = false; break; }
        }
        if (is_accepted) {
            bool is_final = false;
            for (int f = 0; f < final_states.size(); f++) { if (current_state == final_states[f]) { is_final = true; break; } }
            is_accepted = is_final;
        }
        if (is_accepted) cout << "IGEN\n"; else cout << "NEM\n";
    }
    cout << "============================\n" << endl;
    return 0;
}