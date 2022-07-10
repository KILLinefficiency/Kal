#pragma once

#include "errors.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

class VarTable {
    private:
        std::unordered_map<std::string, bool> booleans;
        std::unordered_map<std::string, double> numbers;
        std::unordered_map<std::string, std::string> strings;

        std::unordered_map<std::string, std::string> type_check;

    public:
        std::string get_type(std::string var_name) {
            std::string var_type = type_check[var_name];
            return var_type;
        }

        bool var_exists(std::string var_name) {
            bool exist_condition = (get_type(var_name) != "");
            return exist_condition;
        }

        void var_add(std::string var_type, std::string var_name, std::string var_value, bool declaration = false) {
            if(var_exists(var_name) && declaration) {
                errors::var_redeclare_error(var_name, get_type(var_name));
                exit(1);
            }


            type_check[var_name] = var_type;

            if(var_type == "str") {
                strings[var_name] = var_value;
            }

            else if(var_type == "num") {
                double casted_var_value = std::stod(var_value);
                numbers[var_name] = casted_var_value;
            }

            else if(var_type == "bool") {
                bool boolean_value;
                if(var_value == "true") {
                    boolean_value = true;
                }
                if(var_value == "false") {
                    boolean_value = false;
                }
                booleans[var_name] = boolean_value;
            }
        }

        void var_delete(std::string var_name) {
            std::string var_type = type_check[var_name];

            if(var_type == "str") {
                strings.erase(var_name);
            }
            else if(var_type == "num") {
                numbers.erase(var_name);
            }
            else if(var_type == "bool") {
                booleans.erase(var_name);
            }
        }

        std::string get_from_strings(std::string var_name) {
            std::string var_val = strings[var_name];
            return var_val;
        }

        double get_from_numbers(std::string var_name) {
            double var_val = numbers[var_name];
            return var_val;
        }

        bool get_from_booleans(std::string var_name) {
            bool var_val = booleans[var_name];
            return var_val;
        }

        void read_var(std::string var_name) {
            std::string var_type = get_type(var_name);

            if(var_type == "str") {
                std::getline(std::cin, strings[var_name]);
            }
            else if(var_type == "num") {
                std::cin >> numbers[var_name];
            }
            else if(var_type == "bool") {
                std::cin >> booleans[var_name];
            }
        }

};
