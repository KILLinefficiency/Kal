#include "../lib/lib_style.hpp"
#include "../config.hpp"
#include <cstdint>

int current = 1;
int total = 0;

void component(std::string kal_component) {
    std::cout << style::style["blue"] << style::style["bold"]
        << "\n[*] " << style::style["bold"]
        << "Testing the Kal " << kal_component << "..." << std::endl;
}

void title(std::string function) {
    std::cout << style::style["green"]
        << "[" << current << " / " << total << "] "
        << style::style["reset"] << style::style["italic"] << "Testing " 
        << style::style["reset"] << style::style["bold"]
        << function << style::style["reset"] << style::style["bold"] << "... ";
}

template <typename Type = std::string>
void check(Type found, Type actual, std::string error = "") {
    if(found != actual) {
        std::cerr << style::style["reset"] << style::style["red"]
            << "\nGenerated Value `" << found <<  "` != Actual Value `" << actual << "`.\n";
        if(error != "") {
            std::cerr << style::style["reset"] << style::style["red"] << error << "\n";
        }
        std::cerr << style::style["bold"] << "FAIL." << style::style["reset"] << "\n";
        exit(1);
    }
}

void check_vector(const std::vector<std::string>& found, const std::vector<std::string>& actual, std::string name = "") {
    uint64_t vector_size = found.size();
    uint64_t actual_size = actual.size();

    if(name != "") {
        name = " " + name;
    }

    if(vector_size != actual_size) {
        std::cerr << style::style["reset"] << style::style["yellow"]
            << "\nGenerated" << name << " Size `" << vector_size << "` != Actual" << name << " Size `" << actual_size << "`.\n";
        std::cerr << style::style["bold"] << "WARN." << style::style["reset"] << "\n";
    }

    for(uint64_t i = 0; i < vector_size; i++) {
        if(found[i] != actual[i]) {
            std::cerr << style::style["reset"] << style::style["red"]
                << "\nGenerated" << name << " Value `" << found[i] << "` != Actual" << name << " Value `" << actual[i] << "`.\n"; 

            std::cerr << style::style["bold"] << "FAIL." << style::style["reset"] << "\n";
            exit(1);
        }
    }
}

void check_token(const Token& found, const Token& actual) {
    bool fail = false;

    if(found.head != actual.head) {
        fail = true;
        std::cerr << style::style["reset"] << style::style["red"]
            << "\nGenerated Head `" << found.head << "` != Actual Head `" << actual.head << "`.\n";
    }

    check_vector(found.init, actual.init, "Init");

    check_vector(found.values, actual.values);

    if(found.target != actual.target) {
        fail = true;
        std::cerr << style::style["reset"] << style::style["red"]
            << "\nGenerated Target Value `" << found.target << "` != Actual Target Value `" << actual.target << "`.\n";
    }

    if(fail) {
        std::cerr << style::style["bold"] << "FAIL." << style::style["reset"] << "\n";
        exit(1);
    }
}

void progress() {
    std::cout << style::style["green"] << "PASS.\n";
    current++;
}

void exit_successfully() {
    std::cout << style::style["reset"] << style::style["bold"]
        << "\nAll tests " << style::style["green"] << "PASS" << style::style["reset"]
        << style::style["bold"] << ".\n" << style::style["reset"]
        << std::endl;
}