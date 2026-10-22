#ifndef DFA_H
#define DFA_H

#include "../problem.hpp"
#include "../cxxopts.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// Egyetlen átmenet (nyíl) adatait tároló egyszerű struktúra
struct Transition {
    string from;      
    char symbol;      
    string to;        
};

class DFAProblem : public Problem {
public:
    void initialize_parser(cxxopts::Options& options) override {
        options.add_options()
            ("c,check", "Szavak ellenorzese", cxxopts::value<std::string>());
    }

    // JAVÍTVA: Hozzáadva a const kulcsszó
    bool is_chosen_problem(const cxxopts::ParseResult& args) override {
        return args.count("check") > 0;
    }

    // JAVÍTVA: void helyett int, és const kulcsszó
    int run(const cxxopts::ParseResult& args) override {
        string input_file = args["input"].as<std::string>();
        string output_file = args["output"].as<std::string>();
        string check_words = args["check"].as<std::string>();

        ifstream file(input_file);
        if (!file.is_open()) {
            cout << "Hiba: Nem tudom megnyitni a bemeneti fajlt!" << endl;
            return 1; // Hiba esetén 1-essel térünk vissza
        }

        vector<string> states;
        vector<string> alphabet;
        string start_state;
        vector<string> final_states;
        vector<Transition> transitions;

        string line, word;

        getline(file, line);
        stringstream ss1(line);
        while (ss1 >> word) states.push_back(word);

        getline(file, line);
        stringstream ss2(line);
        while (ss2 >> word) alphabet.push_back(word);

        getline(file, line);
        stringstream ss3(line);
        ss3 >> start_state;

        getline(file, line);
        stringstream ss4(line);
        while (ss4 >> word) final_states.push_back(word);

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            Transition t;
            ss >> t.from >> t.symbol >> t.to;
            transitions.push_back(t);
        }
        file.close();

        vector<string> words_to_test;
        stringstream ss_words(check_words);
        string current_word;
        while (getline(ss_words, current_word, ',')) {
            words_to_test.push_back(current_word);
        }

        ofstream out(output_file);

        for (int i = 0; i < words_to_test.size(); i++) {
            string w = words_to_test[i];
            string current_state = start_state;
            bool is_accepted = true;

            for (int j = 0; j < w.length(); j++) {
                char letter = w[j];
                bool found_rule = false;

                for (int k = 0; k < transitions.size(); k++) {
                    if (transitions[k].from == current_state && transitions[k].symbol == letter) {
                        current_state = transitions[k].to;
                        found_rule = true;
                        break;
                    }
                }

                if (!found_rule) {
                    is_accepted = false;
                    break;
                }
            }

            if (is_accepted) {
                bool is_final = false;
                for (int f = 0; f < final_states.size(); f++) {
                    if (current_state == final_states[f]) {
                        is_final = true;
                        break;
                    }
                }
                is_accepted = is_final;
            }

            if (is_accepted) {
                out << "IGEN\n";
            } else {
                out << "NEM\n";
            }
        }
        out.close();

        return 0; // JAVÍTVA: Sikeres futás végén 0-val térünk vissza
    }
};

#endif