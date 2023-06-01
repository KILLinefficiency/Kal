#pragma once

#include "../lexer.hpp"
#include "../variable.hpp"

namespace lib {
    void copy_list(std::string from_list, std::string to_list, VarTable& var) {
        int list_size = var.get_list_size(from_list);
        std::string list_type = var.get_structure_type(from_list);
        var.add_structure(to_list, list_type);
        list_type = list_type.substr(0, 3);

        var.var_add("var", "[" + to_list + "#len]", std::to_string(list_size));
        for(int copy_itr = 0; copy_itr < list_size; copy_itr++) {
            std::string each_value = "";
            std::string index = std::to_string(copy_itr);
            std::string from_identifier = "[" + from_list + "#" + index + "]";
            std::string to_identifier = "[" + to_list + "#" + index + "]";
            if(list_type == "str") {
                each_value = var.get_from_strings(from_identifier);
            }
            else if(list_type == "num") {
                each_value = std::to_string(var.get_from_numbers(from_identifier));
            }

            var.var_add("var", to_identifier, each_value);
        }
    }

    void create_list(std::vector<std::string>& list_data, VarTable& var) {
        std::string& list_type = list_data[0];
        std::string& list_name = list_data[1];
        int list_len = list_data.size();

        if(list_len == 3 && list_data[2][0] == '$') {
            std::string from_list = list_data[2].substr(1);
            copy_list(from_list, list_name, var);
            return;
        }

        var.var_add("var", "[" + list_name + "#len]", std::to_string(list_len - 2));
        for(int each_item = 2; each_item < list_len; each_item++) {
            std::string identifier = "[" + list_name + "#" + std::to_string(each_item - 2) + "]";
            if(list_data[each_item][0] == '$') {
                list_data[each_item] = var.eval_var(list_data[each_item]);
            }
            var.var_add("var", identifier, list_data[each_item]);
        }
        var.add_structure(list_name, list_type + "_list");
    }

    void get_list_size(std::string& list_name, std::string& target_var, VarTable& var) {
        list_name = list_name.substr(1);
        std::string struct_type = var.get_structure_type(list_name);
        if(struct_type == "str_list" || struct_type == "num_type") {
            double struct_size = var.get_from_numbers("[" + list_name + "#len]");
            var.var_add("var", target_var, std::to_string(struct_size));
        }
    }

    void reassign_list(std::string to_list, std::string from_list, VarTable& var) {
        int to_list_size = var.get_from_numbers("[" + to_list + "#len]");
        int from_list_size = var.get_from_numbers("[" + from_list + "#len]");
        copy_list(from_list, to_list, var);
        if(from_list_size < to_list_size) {
            for(int del_start = from_list_size; del_start < to_list_size; del_start++) {
                var.var_delete("[" + to_list + "#" + std::to_string(del_start) + "]");
            }
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
        std::string list_type = var.get_structure_type(push_list).substr(0, 3);
        var.var_add("var", identifier, push_item);
        var.var_add("var", len_var, std::to_string(latest_index + 1));
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

        var.var_add("var", target_str, formed_string);
    }

    void unpack_list(std::string& unpack_list, std::vector<std::string>& list_vars, VarTable& var) {
        int total_unpack_vars = list_vars.size();
        for(int unpack_itr = 0; unpack_itr < total_unpack_vars; unpack_itr++) {
            if(list_vars[unpack_itr][0] == '$') {
                list_vars[unpack_itr] = var.expand_var(list_vars[unpack_itr]);
            }
            std::string list_element_id = "$" + unpack_list + "#" + std::to_string(unpack_itr);
            std::string unpack_value = var.eval_var(list_element_id);
            var.var_add("var", list_vars[unpack_itr], unpack_value);
        }
    }

    void reverse_list(std::string list_name, VarTable& var) {
        std::string list_type = var.get_structure_type(list_name).substr(0, 3);
        int len = int(var.get_from_numbers("[" + list_name + "#len]"));
        int total_itr = len / 2;

        std::string item = "";
        std::string other_item = "";

        for(int index = 0; index < total_itr; index++) {
            int other_index = len - index - 1;
            std::string identifier = "[" + list_name + "#" + std::to_string(index) + "]";
            std::string other_identifier = "[" + list_name + "#" + std::to_string(other_index) + "]";
            if(list_type == "num") {
                item = std::to_string(var.get_from_numbers(identifier));
                other_item = std::to_string(var.get_from_numbers(other_identifier));
            }
            else if(list_type == "str") {
                item = var.get_from_strings(identifier);
                other_item = var.get_from_strings(other_identifier);
            }

            var.var_add("var", identifier, other_item);
            var.var_add("var", other_identifier, item);
        }
    }

    void range_to_list(std::vector<double>& range_data, std::string& target_list, VarTable& var) {
        int range_items = 0;
        target_list = target_list.substr(1);

        double range_step = 1;
        if(range_data.size() == 3) {
            range_step = range_data[2];
        }
        for(double& range_itr = range_data[0]; range_itr <= range_data[1]; range_itr += range_step) {
            var.var_add("var", "[" + target_list + "#" + std::to_string(range_items) + "]", std::to_string(range_itr));
            range_items++;
        }

        var.var_add("var", "[" + target_list + "#len]", std::to_string(range_items));
        var.add_structure(target_list, "num_list");
    }
}