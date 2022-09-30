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

    void get_list_size() {}
}