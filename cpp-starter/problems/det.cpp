#include "det.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <map>

using namespace std;

void DetProblem::initialize_parser(cxxopts::Options& options) {
    options.add_options()("d,det", "Automata determinizalasa", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
}

bool DetProblem::is_chosen_problem(const cxxopts::ParseResult& args) {
    return args.count("det") > 0;
}

int DetProblem::run(const cxxopts::ParseResult& args) {
    cout << "SIKERES INDULAS: 2. feladat (Determinizalas) fut..." << endl;
    string input_file = args["input"].as<std::string>();

    ifstream file(input_file);
    if (!file.is_open()) { cout << "Hiba: Nem tudom megnyitni a bemeneti fajlt!" << endl; return 1; }

    set<string> valid_states; set<string> valid_alphabet; vector<string> alphabet_list;
    string start_state; set<string> final_states; map<pair<string, string>, set<string>> nfa_trans;
    string line, word;

    if (getline(file, line)) { stringstream ss(line); while (ss >> word) valid_states.insert(word); }
    if (getline(file, line)) { stringstream ss(line); while (ss >> word) { valid_alphabet.insert(word); alphabet_list.push_back(word); } }
    if (getline(file, line)) { stringstream ss(line); ss >> start_state; }
    if (getline(file, line)) { stringstream ss(line); while (ss >> word) final_states.insert(word); }

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line); string from, to, symbol;
        if (ss >> from >> symbol >> to) nfa_trans[{from, symbol}].insert(to);
    }
    file.close();

    map<set<string>, string> dfa_state_names; vector<set<string>> queue; vector<string> dfa_states_list;
    int state_counter = 0;

    set<string> start_set = {start_state};
    string start_name = "s" + to_string(state_counter++);
    dfa_state_names[start_set] = start_name; queue.push_back(start_set); dfa_states_list.push_back(start_name);

    map<pair<string, string>, string> dfa_trans;

    int q_idx = 0;
    while (q_idx < queue.size()) {
        set<string> current = queue[q_idx++]; string current_name = dfa_state_names[current];
        for (const string& c : alphabet_list) {
            set<string> next_set;
            for (const string& q : current) {
                if (nfa_trans.count({q, c})) { for (const string& dest : nfa_trans[{q, c}]) next_set.insert(dest); }
            }
            if (!next_set.empty()) {
                if (dfa_state_names.find(next_set) == dfa_state_names.end()) {
                    string new_name = "s" + to_string(state_counter++);
                    dfa_state_names[next_set] = new_name; queue.push_back(next_set); dfa_states_list.push_back(new_name);
                }
                dfa_trans[{current_name, c}] = dfa_state_names[next_set];
            }
        }
    }

    vector<string> dfa_finals;
    for (const auto& dfa_state : queue) {
        bool is_final = false;
        for (const string& nfa_s : dfa_state) { if (final_states.find(nfa_s) != final_states.end()) { is_final = true; break; } }
        if (is_final) dfa_finals.push_back(dfa_state_names[dfa_state]);
    }

    cout << "\n=== 2. FELADAT (KIMENET) ===" << endl;
    for (int i = 0; i < dfa_states_list.size(); i++) cout << dfa_states_list[i] << (i == dfa_states_list.size() - 1 ? "" : " "); cout << "\n";
    for (int i = 0; i < alphabet_list.size(); i++) cout << alphabet_list[i] << (i == alphabet_list.size() - 1 ? "" : " "); cout << "\n";
    cout << "s0\n";
    for (int i = 0; i < dfa_finals.size(); i++) cout << dfa_finals[i] << (i == dfa_finals.size() - 1 ? "" : " "); cout << "\n";
    for (const auto& t : dfa_trans) cout << t.first.first << " " << t.first.second << " " << t.second << "\n";
    cout << "============================\n" << endl;

    return 0;
}
