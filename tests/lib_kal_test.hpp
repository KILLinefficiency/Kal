#include "../lib/lib_style.hpp"

int current = 1;
int total = 0;

void component(std::string kal_component) {
    std::cout << style::style["green"] << style::style["bold"] 
        << "\n[*] " << style::style["reset"] << style::style["bold"]
        << "Testing the Kal " << kal_component << "..." << std::endl;
}

void title(std::string function) {
    std::cout << style::style["green"]
        << "[" << current << " / " << total << "] "
        << style::style["reset"] << style::style["italic"] << "Testing " 
        << style::style["reset"] << style::style["bold"]
        << function << style::style["reset"] << style::style["bold"] << "... ";
}

void check(std::string found, std::string actual, std::string error = "") {
    if(found != actual) {
        std::cerr << style::style["reset"] << style::style["red"]
            << "\nGenerated Value `" << found << "`" << " != " << "Actual Value `" << actual << "`.\n";
        if(error != "") {
            std::cerr << style::style["reset"] << style::style["red"] << error << "\n";
        }
        std::cout << style::style["bold"] << "FAIL." << style::style["reset"] << "\n";
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