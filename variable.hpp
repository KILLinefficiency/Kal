#pragma once

#include "errors.hpp"
#include "lib/lib_math.hpp"

#include <iostream>
#include <string>
#include <unordered_map>

class VarTable {
    private:
        std::unordered_map<std::string, double> numbers;
        std::unordered_map<std::string, std::string> strings;


        std::unordered_map<std::string, std::string> structures;

        std::unordered_map<std::string, std::string> type_check;
        std::unordered_map<std::string, std::string> mem_check;

    public:
        std::unordered_map<std::string, std::string> data;
        std::string get_type(std::string var_name) {
            std::string var_type = type_check[var_name];
            /*if(var_type == "") {
                errors::undefined_var_error(var_name);
            }*/
            return var_type;
        }

        std::string get_mem_type(std::string var_name) {
            std::string var_mem_type = mem_check[var_name];
            return var_mem_type;
        }

        bool var_exists(std::string var_name) {
            bool exist_condition = (get_type(var_name) != "");
            return exist_condition;
        }

        void var_add(std::string mem_type, std::string var_name, std::string var_value, bool declaration = false) {
            if(var_exists(var_name) && declaration) {
                errors::var_redeclare_error(var_name, get_type(var_name));
            }

            if(var_exists(var_name) && get_mem_type(var_name) == "const") {
                errors::change_const_var_error(var_name);
            }

            // type_check[var_name] = var_type;
            mem_check[var_name] = mem_type;

            data[var_name] = var_value;

            /*if(var_type == "str") {
                strings[var_name] = var_value;
            }

            else if(var_type == "num") {
                double casted_var_value = std::stod(var_value);
                numbers[var_name] = casted_var_value;
            }*/

            /*else {
                errors::unknown_var_type(var_name, var_type);
            }*/
        }

        void var_delete(std::string var_name) {
            std::string var_type = get_type(var_name);

            type_check.erase(var_name);
            if(var_type == "str") {
                strings.erase(var_name);
            }
            else if(var_type == "num") {
                numbers.erase(var_name);
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

        void read_var(std::string var_name) {
            std::string temp_data;
            std::getline(std::cin, temp_data);
            data[var_name.substr(1)] = '"' + temp_data + '"';
        }

        void add_structure(std::string struct_name, std::string struct_type) {
            structures[struct_name] = struct_type;
        }

        std::string get_structure_type(std::string struct_name) {
            return structures[struct_name];
        }

        int get_list_size(std::string list_name) {
            return int(get_from_numbers("[" + list_name + "#len]"));
        }

        /*std::string eval_var(std::string& var_expr) {
            var_expr = var_expr.substr(1);
            std::vector<std::string> var_params = lib::split(var_expr, '#');
            int var_params_size = var_params.size();
            std::string var_type = type_check[var_params[0]];
            if(var_params_size == 1) {
                if(var_type == "num") {
                    return lib::display_num(get_from_numbers(var_params[0]));
                }
                else if(var_type == "str") {
                    return get_from_strings(var_params[0]);
                }
            }
            else if(var_params_size == 2) {
                std::string list_type = get_structure_type(var_params[0]);
                std::string index = var_params[1];
                if(var_params[1][0] == '$') {
                    index = std::to_string(int(get_from_numbers(lexer::get_var_name_from_token(var_params[1]))));
                }
                std::string identifier = "[" + var_params[0] + "#" + index + "]";
                if(list_type == "num_list") {
                    return lib::display_num(get_from_numbers(identifier));
                }
                return get_from_strings(identifier);
            }

            return "";
        }*/

        std::string eval_var(std::string var_expr) {
            std::string val = data[var_expr];
            if(var_expr[0] == '$') {
                val = data[var_expr.substr(1)];
            }
            if(val[0] == '"') {
                val = val.substr(1, val.size() - 2);
            }
            if(val == "") {
                std::cout << val << "does not exist" << std::endl;
                exit(1);
            }

            return val;
        }

        std::string expand_var(std::string& var_expr) {
            var_expr = var_expr.substr(1);
            std::string final_var = var_expr;
            if(var_expr.find('#') != std::string::npos) {
                final_var = "[";
                std::vector<std::string> var_params = lib::split(var_expr, '#');
                int total_params = var_params.size();
                for(int param_itr = 0; param_itr < total_params; param_itr++) {
                    if(var_params[param_itr][0] == '$') {
                        var_params[param_itr] = eval_var(var_params[param_itr]);
                    }
                    final_var += var_params[param_itr] + "#";
                }
                final_var[final_var.size() - 1] = ']';
            }
            return final_var;
        }

        std::string print_list(std::string list_name) {
            std::string list_type = get_structure_type(list_name);
            std::string list_result = "[";
            int list_len = get_list_size(list_name);
            std::string list_end = ", ";
            for(int each_item = 0; each_item < list_len; each_item++) {
                std::string identifier = "[" + list_name + "#" + std::to_string(each_item) + "]";
                if(each_item == list_len - 1) {
                    list_end = "";
                }
                if(list_type == "str_list") {
                    list_result += (get_from_strings(identifier) + list_end);
                }
                else if(list_type == "num_list") {
                    list_result += (lib::display_num(get_from_numbers(identifier)) + list_end);
                }
            }
            list_result += "]";
            return list_result;
        }
};
