#include "mini.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <cctype>

using namespace std;

string to_s_name(const string& q_name) {
    if (!q_name.empty() && q_name[0] == 'q') {
        return "s" + q_name.substr(1);
    }
    return "s" + q_name;
}

bool compare_states(const string& a, const string& b) {
    if (a.size() > 1 && b.size() > 1 && isdigit(a[1]) && isdigit(b[1])) {
        try {
            int num_a = stoi(a.substr(1));
            int num_b = stoi(b.substr(1));
            if (num_a != num_b) return num_a < num_b;
        } catch (...) {}
    }
    return a < b;
}

void MiniProblem::initialize_parser(cxxopts::Options& options) {
    options.add_options()
        ("m,mini", "Automata minimalizalasa", cxxopts::value<bool>()->default_value("false")->implicit_value("true"));
}

bool MiniProblem::is_chosen_problem(const cxxopts::ParseResult& args) {
    return args.count("mini") > 0;
}

int MiniProblem::run(const cxxopts::ParseResult& args) {
    string input_file = args["input"].as<string>();
    string output_file = args["output"].as<string>();

    ifstream file(input_file);
    if (!file.is_open()) return 1;

    set<string> valid_states, valid_alphabet, final_states;
    vector<string> states_list, alphabet_list;
    string start_state, line, word;

    // allapotok
    if (getline(file, line)) {
        stringstream ss(line);
        while (ss >> word) {
            if (valid_states.insert(word).second) states_list.push_back(word);
        }
    }
    //  abc
    if (getline(file, line)) {
        stringstream ss(line);
        while (ss >> word) {
            if (valid_alphabet.insert(word).second) alphabet_list.push_back(word);
        }
    }
    // kezdo allapot
    if (getline(file, line)) {
        stringstream ss(line);
        ss >> start_state;
        if (valid_states.find(start_state) == valid_states.end()) return 1;
    }
    // vegallapot
    if (getline(file, line)) {
        stringstream ss(line);
        while (ss >> word) {
            if (valid_states.find(word) != valid_states.end()) {
                final_states.insert(word);
            }
        }
    }

    // atmenetek beolvasasa
    map<pair<string, string>, string> trans;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string from, symbol, to;
        if (ss >> from >> symbol >> to) {
            if (valid_states.count(from) && valid_alphabet.count(symbol) && valid_states.count(to)) {
                trans[{from, symbol}] = to;
            }
        }
    }
    file.close();

    // elerheto allapotok bfs el
    set<string> reachable;
    vector<string> q_bfs;
    if (!start_state.empty()) {
        reachable.insert(start_state);
        q_bfs.push_back(start_state);
    }
    size_t head = 0;
    while (head < q_bfs.size()) {
        string curr = q_bfs[head++];
        for (const string& c : alphabet_list) {
            auto it = trans.find({curr, c});
            if (it != trans.end() && reachable.find(it->second) == reachable.end()) {
                reachable.insert(it->second);
                q_bfs.push_back(it->second);
            }
        }
    }

    vector<string> r_states(reachable.begin(), reachable.end());
    sort(r_states.begin(), r_states.end(), compare_states);

    // minimalizalas
    map<string, int> group;
    for (const string& s : r_states) {
        group[s] = final_states.count(s) ? 1 : 0;
    }

    bool changed = true;
    while (changed) {
        changed = false;
        map<string, int> new_group;
        map<vector<int>, int> sig_to_id;
        int next_id = 0;

        for (const string& s : r_states) {
            vector<int> sig;
            sig.push_back(group[s]);
            for (const string& c : alphabet_list) {
                auto it = trans.find({s, c});
                if (it != trans.end() && reachable.count(it->second)) {
                    sig.push_back(group[it->second]);
                } else {
                    sig.push_back(-1); // Nincs átmenet
                }
            }
            if (sig_to_id.find(sig) == sig_to_id.end()) {
                sig_to_id[sig] = next_id++;
            }
            new_group[s] = sig_to_id[sig];
        }

        if (new_group != group) {
            int old_max = 0, new_max = 0;
            for (const auto& p : group) old_max = max(old_max, p.second);
            for (const auto& p : new_group) new_max = max(new_max, p.second);
            group = new_group;
            if (new_max > old_max) changed = true;
        }
    }

    // Csoportok osszegyujtese es elnevezese
    map<int, vector<string>> partitions;
    for (const string& s : r_states) {
        partitions[group[s]].push_back(s);
    }

    map<string, string> state_to_min_name;
    vector<string> min_states;

    for (auto& p : partitions) {
        vector<string>& members = p.second;
        sort(members.begin(), members.end(), compare_states);
        string rep_name = to_s_name(members[0]);
        min_states.push_back(rep_name);
        for (const string& m : members) {
            state_to_min_name[m] = rep_name;
        }
    }

    sort(min_states.begin(), min_states.end(), compare_states);

    // kezdo es vegallapot atnevezese
    string min_start = state_to_min_name[start_state];

    set<string> min_finals_set;
    for (const string& f : final_states) {
        if (reachable.count(f)) {
            min_finals_set.insert(state_to_min_name[f]);
        }
    }
    vector<string> min_finals(min_finals_set.begin(), min_finals_set.end());
    sort(min_finals.begin(), min_finals.end(), compare_states);

    // atmenetek eloallitasa (duplikaciok nelkul es abc sorrendben)
    set<tuple<string, string, string>> min_trans;
    for (const auto& t : trans) {
        string u = t.first.first;
        string c = t.first.second;
        string v = t.second;
        if (reachable.count(u) && reachable.count(v)) {
            min_trans.insert({state_to_min_name[u], c, state_to_min_name[v]});
        }
    }

    // Kiiras fileba
    ofstream out(output_file);
    if (!out.is_open()) return 1;

    for (size_t i = 0; i < min_states.size(); i++) {
        out << min_states[i] << (i + 1 == min_states.size() ? "" : " ");
    }
    out << "\n";

    for (size_t i = 0; i < alphabet_list.size(); i++) {
        out << alphabet_list[i] << (i + 1 == alphabet_list.size() ? "" : " ");
    }
    out << "\n";

    out << min_start << "\n";

    for (size_t i = 0; i < min_finals.size(); i++) {
        out << min_finals[i] << (i + 1 == min_finals.size() ? "" : " ");
    }
    out << "\n";

    for (const auto& mt : min_trans) {
        out << get<0>(mt) << " " << get<1>(mt) << " " << get<2>(mt) << "\n";
    }

    out.close();
    return 0;
}