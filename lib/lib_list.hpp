#pragma once

#include "../lexer.hpp"
#include "../variable.hpp"

namespace lib {
    void create_list(std::vector<std::string>& list_data, VarTable& var) {
        std::string& list_type = list_data[0];
        std::string& list_name = list_data[1];
        int list_len = list_data.size();
        var.var_add("var", "num", "[" + list_name + "#len]", std::to_string(list_len - 2));
        for(int each_item = 2; each_item < list_len; each_item++) {
            std::string identifier = "[" + list_name + "#" + std::to_string(each_item - 2) + "]";
            if(list_data[each_item][0] == '$') {
                list_data[each_item] = var.eval_var(list_data[each_item]);
            }
            var.var_add("var", list_type, identifier, list_data[each_item]);
        }
        var.add_structure(list_name, list_type + "_list");
    }

    void get_list_size(std::string& list_name, std::string& target_var, VarTable& var) {
        list_name = list_name.substr(1);
        std::string struct_type = var.get_structure_type(list_name);
        if(struct_type == "str_list" || struct_type == "num_type") {
            double struct_size = var.get_from_numbers("[" + list_name + "#len]");
            var.var_add("var", "num", target_var, std::to_string(struct_size));
        }
    }

    void push_to_list(std::vector<std::string>& push_data, VarTable& var) {
        if(push_data[0][0] == '$') {
            push_data[0] = var.eval_var(push_data[0]);
        }
        std::string& push_item = push_data[0];
        std::string push_list = push_data[1].substr(1);
        std::string len_var = "[" + push_list + "#len]";
        int latest_index = var.get_list_size(push_list);

        std::string identifier = "[" + push_list + "#" + std::to_string(latest_index) + "]";
        var.var_add("var", "str", identifier, push_item);
        var.var_add("var", "num", len_var, std::to_string(latest_index + 1));
    }

    void join_list(std::string& list_name, std::string& join_text, std::string& target_str, VarTable& var) {
        std::string formed_string = "";
        int list_size = var.get_list_size(list_name);
        std::string struct_type = var.get_structure_type(list_name).substr(0, 3);
        for(int item_itr = 0; item_itr < list_size; item_itr++) {
            if(item_itr == list_size - 1) {
                join_text = "";
            }

            if(struct_type == "str") {
                formed_string += (var.get_from_strings("[" + list_name + "#" + std::to_string(item_itr) + "]") + join_text);
            }
            else if(struct_type == "num") {
                formed_string += (std::to_string(var.get_from_numbers("[" + list_name + "#" + std::to_string(item_itr) + "]")) + join_text);
            }
        }

        var.var_add("var", struct_type, target_str, formed_string);
    }

    void unpack_list(std::string& unpack_list, std::string& unpack_list_type, std::vector<std::string>& list_vars, VarTable& var) {
        int total_unpack_vars = list_vars.size();
        for(int unpack_itr = 0; unpack_itr < total_unpack_vars; unpack_itr++) {
            if(list_vars[unpack_itr][0] == '$') {
                list_vars[unpack_itr] = var.expand_var(list_vars[unpack_itr]);
            }
            std::string list_element_id = "$" + unpack_list + "#" + std::to_string(unpack_itr);
            std::string unpack_value = var.eval_var(list_element_id);
            var.var_add("var", unpack_list_type, list_vars[unpack_itr], unpack_value);
        }
    }
}