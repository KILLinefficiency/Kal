#pragma once

#include "config.hpp"
#include "lexer.hpp"
#include "errors.hpp"
#include "expr_parser.hpp"
#include "var.hpp"
#include "lib/lib_style.hpp"
#include "lib/lib_string.hpp"
#include "lib/lib_list.hpp"
#include "lib/lib_dict.hpp"
#include "lib/lib_serialize.hpp"

#include <stack>
#include <utility>
#include <tuple>

#define EXPECT(ARGS) if(cmd_size != ARGS) { errors::expected_arguments(globals, ins, ARGS); }

namespace parser {
    void std_out(std::string out_text, Globals& globals) {
        if(parser::is_var(out_text)) {
            std::cout << lib::resolve_string(VarTable::print(out_text, globals));
            return;
        }
        else if(out_text[0] == '$' || out_text[0] == '(') {
            std::cout << lib::resolve_string(eval(out_text, globals));
            return;
        }
        std::cout << lib::resolve_string(lib::render_escape_chars(out_text));
    }

    void std_err(std::string err_text) {
        std::cerr << lib::resolve_string(lib::render_escape_chars(err_text));
    }
}

void exec_defer(Globals& globals) {
    int& depth = globals.depth;
    DeferStack& defer_stack = globals.defer_stack;

    while(!defer_stack.empty() && defer_stack.top().second >= depth) {
        eval(defer_stack.top().first, globals);
        defer_stack.pop();
    }
}

void spread_values(std::string& operand, std::vector<std::string>& values, uint64_t& index, Globals& globals) {
    if((operand[0] != '[') && ((operand[0] == '#' && operand[1] == '(') || dynamic_cast<Dict*>(VarTable::get(operand, {}, true, true, true, globals)))) {
        return;
    }
    Value* value = make_value(operand, globals);
    List* args = dynamic_cast<List*>(value);
    values[index] = args->items[0]->print();
    int size = args->items.size();
    values.reserve(values.size() + size);
    for(int arg_index = 1; arg_index < size; arg_index++) {
        values.insert(values.begin() + (index + arg_index), args->items[arg_index]->print());
    }
    delete args;
}

Value* line_exec(std::vector<Token>& tokens, bool auto_return, bool fn_defer, bool top_return, Globals& globals) {
    int& depth = globals.depth;
    DeferStack& defer_stack = globals.defer_stack;

    bool warn = true;
    int total_tokens = tokens.size();

    int line = 0;
    int current_depth = 0;
    std::stack<std::pair<bool, int>> conditional_stack;
    std::stack<std::pair<int, std::vector<std::string>>> init_loop;
    std::stack<std::tuple<bool, int, int>> loop_stack;
    std::stack<std::tuple<Value*, std::string, int, uint64_t, bool>> range_stack;

    while(line < total_tokens) {
        Token& cmd = tokens[line];
        globals.current_line = tokens[line].line;
        bool cmd_values_modified = false;
        std::vector<std::string> current_cmd_values;
        std::string& ins = cmd.head;
        if(ins[0] == '#' && ins[1] == '!') {
            line++;
            continue;
        }

        for(uint64_t i = 0; i < cmd.values.size(); i++) {
            if(cmd.values[i][0] == '.' && cmd.values[i][1] == '.' && cmd.values[i][2] == '.') {
                if(!cmd_values_modified) {
                    cmd_values_modified = true;
                    current_cmd_values = cmd.values;
                }
                std::string operand = cmd.values[i].substr(3);
                spread_values(operand, cmd.values, i, globals);
            }
        }

        int cmd_size = cmd.values.size();

        if(cmd.head == "<-") {
            bool is_call_stack_empty = globals.call_stack.empty();
            if(is_call_stack_empty && !top_return) {
                // ERR:
                errors::top_return(globals);
            } 
            exec_defer(globals);
            std::string result = eval(cmd.target, globals);
            Value* return_value = nullptr;

            Value* existing_value = VarTable::get(result, {}, true, true, true, globals);
            bool value_exists = (existing_value != nullptr);

            return_value = (parser::is_var(result) && value_exists) ? copy(existing_value) : make_value(result, globals);
            if(!is_call_stack_empty) {
                int original_depth = globals.call_stack.top().second;
                while(depth != original_depth) {
                    VarTable::gc(globals);
                    depth--;
                }
            }

            if((result[0] == '&') || (TO_REF(return_value) && globals.scope[result] == globals.depth)) {
                // ERR:
                errors::invalid_ref(globals, result);
            }
            return return_value;
        }

        if(cmd.head == "{") {
            depth++;
        }

        if(cmd.head[0] == ':') {
            std::string fn_name = cmd.head.substr(1);
            Fn* fn = Functions::fn[fn_name];
            if(fn == nullptr) {
                errors::undefined_function(globals, fn_name);
            }
            int args_size = cmd.values.size();

            bool is_variadic = false;
            int last_arg = fn->init.size() - 2;
            if((last_arg >= 0) && (fn->init[last_arg][0] == '.')) {
                last_arg /= 2;
                is_variadic = true;
            }

            depth++;
            int arg;
            int all = (fn->init.size() / 2);
            if(!is_variadic && (args_size > all)) {
                errors::fn_extra_args(globals, fn_name);
            }
            for(arg = 0; arg < args_size; arg++) {
                if(fn->init[arg * 2][0] == '.') {
                    break;
                }
                if(is_variadic && (arg >= last_arg)) {
                    break;
                }
                std::string& r_val = cmd.values[arg];
                if(r_val[0] == '.') {
                    std::string operand = r_val.substr(3);
                    Dict* dict_value = dynamic_cast<Dict*>(make_value(operand, globals));
                    for(const std::string& key : dict_value->keys) {
                        Value* value = dict_value->dict[key];
                        VarTable::set(key, "", value, VAR, false, depth, true, globals);
                    }
                    delete dict_value;
                    arg++;
                    continue;
                }
                VarTable::set(fn->init[arg * 2], r_val, nullptr, VAR, false, depth, true, globals);
            }

            int arg_count = arg;
            for(int rest = arg; rest < all; rest++) {
                if(is_variadic && (rest >= last_arg)) {
                    break;
                }
                std::string& r_val = fn->init[(rest * 2) + 1];
                if(r_val != "undef") {
                    arg_count++;
                }
                if(r_val[0] == '(') {
                    r_val = eval(r_val, globals);
                }
                VarTable::set(fn->init[rest * 2], r_val, nullptr, VAR, false, depth, true, globals);
            }

            int i = last_arg;
            if(is_variadic) {
                std::string variadic_arg = fn->init[last_arg * 2].substr(3);
                List* variadic_values = new List();
                int to_reserve = args_size - last_arg;
                if(to_reserve > 0) {
                    variadic_values->items.reserve(to_reserve);
                }
                for(i = last_arg; i < args_size; i++) {
                    variadic_values->items.emplace_back(make_value(cmd.values[i], globals));
                }
                VarTable::set(variadic_arg, "", variadic_values, VAR, true, depth, true, globals);
            }

            if(!is_variadic && (arg_count < all)) {
                errors::fn_less_args(globals, fn_name);
            }

            globals.call_stack.push(std::pair<std::string, int> { fn_name, depth });
            Value* return_value = line_exec(fn->body, false, true, false, globals);
            if(!auto_return) {
                if(return_value == nullptr) {
                    return_value = new Null();
                }
                if(cmd.target != "") {
                    bool allow_shadowing = VarTable::get(cmd.target, {}, true, true, true, globals) != nullptr;
                    globals.depth--;
                    VarTable::set(cmd.target, "", return_value, VAR, true, depth - 1, allow_shadowing, globals);
                    globals.depth++;
                }
                else {
                    delete return_value;
                }
            }

            VarTable::gc(globals);
            depth--;
            globals.call_stack.pop();
            if(auto_return) {
                return return_value;
            }
        }

        else if(ins == "" && cmd.init.size() > 0) {
            int total_reassigns = cmd.init.size();
            for(int each_reassign = 0; each_reassign < total_reassigns; each_reassign += 2) {
                VarTable::set(cmd.init[each_reassign], cmd.init[each_reassign + 1], nullptr, VAR, false, depth, false, globals);
            }
            line++;
            continue;
        }

        if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") {
            depth += 1;
            current_depth = depth;
            if(tokens[line].head == "if") {
                std::string value = eval(tokens[line].values[0], globals);
                if(parser::is_var(value)) {
                    value = VarTable::print(value, globals);
                }
                bool condition = value == "1";
                conditional_stack.push({ condition, depth });
                if(conditional_stack.top().first && tokens[line].head != "else") {
                    line++;
                    continue;
                }
                else {
                    while(depth != current_depth - 1) {
                        line++;
                        if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                        if(tokens[line].head == "}") { depth--; }
                    }
                }
            }
            else if(tokens[line].head == "elif") {
                std::pair<bool, int> check = conditional_stack.top();
                if(check.second == depth) {
                    if(!check.first) {
                        bool condition = eval(tokens[line].values[0], globals) == "1";
                        if(condition && tokens[line].head != "else") {
                            conditional_stack.pop();
                            conditional_stack.push({ condition, depth });
                            line++;
                            continue;
                        }
                        else {
                            while(depth != current_depth - 1) {
                                line++;
                                if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                                if(tokens[line].head == "}") { depth--; }
                            }
                        }
                    }
                    else {
                        while(depth != current_depth - 1) {
                            line++;
                            if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                            if(tokens[line].head == "}") { depth--; }
                        }
                    }
                }
            }
            else if(tokens[line].head == "else") {
                if(conditional_stack.empty()) {
                    errors::invalid_else(globals);
                }
                std::pair<bool, int> check = conditional_stack.top();
                if(check.second == depth) {
                    conditional_stack.pop();
                    if(!check.first) {
                        line++;
                        depth++;
                        continue;
                    }
                    else {
                        while(depth != current_depth - 1) {
                            line++;
                            if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                            if(tokens[line].head == "}") { depth--; }
                        }
                    }
                }
                else {
                    while(depth != current_depth - 1) {
                        line++;
                        if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                        if(tokens[line].head == "}") { depth--; }
                    }
                }
            }
            else if(tokens[line].head == "loop") {
                if(tokens[line].values[0] == "in") {
                    if(range_stack.empty() || (!range_stack.empty() && (std::get<2>(range_stack.top()) != depth))) {
                        std::string var = tokens[line].values[1];
                        bool is_ref = var[0] == '&';
                        if(is_ref) {
                            var = var.substr(1);
                        }

                        std::string r_val = eval(tokens[line].values[2], globals);
                        Value* collection = (is_ref) ?
                            VarTable::get(r_val, {}, true, true, true, globals) :
                            make_value(r_val, globals);

                        if(TO_REF(collection)) {
                            collection = TO_REF(collection)->ref;
                        }

                        uint64_t index = 0;
                        bool condition = index < TO_LIST(collection)->items.size();
                        if(is_ref) {
                            VarTable::set(var, "", new Ref(TO_LIST(collection)->items[index]), VAR, true, depth, false, globals);
                        }
                        else {
                            VarTable::set(var, "", TO_LIST(collection)->items[index], VAR, false, depth, true, globals);
                        }
                        range_stack.push({ collection, var, depth, index, is_ref });
                        loop_stack.push({ condition, line, depth });
                    }
                    else {
                        std::tuple<Value*, std::string, int, uint64_t, bool> top_range = range_stack.top();
                        range_stack.pop();
                        std::get<3>(top_range) += 1;
                        uint64_t index = std::get<3>(top_range);

                        List* list = TO_LIST(std::get<0>(top_range));
                        bool condition = index < list->items.size();

                        if(!condition) {
                            int local_depth = 1;
                            while(local_depth != 0) {
                                line++;
                                if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { local_depth++; }
                                if(tokens[line].head == "}") { local_depth--; }
                            }
                            line++;
                            depth--;
                            if(!std::get<4>(top_range)) {
                                delete std::get<0>(top_range);
                            }
                            continue;
                        }
                        else {
                            if(std::get<4>(top_range)) {
                                VarTable::set(std::get<1>(top_range), "", new Ref(list->items[index]), VAR, true, depth, false, globals);
                            }
                            else {
                                VarTable::set(std::get<1>(top_range), "", list->items[index], VAR, false, depth, true, globals);
                            }
                            range_stack.push(top_range);
                            loop_stack.push({ condition, line, depth });
                        }
                    }
                    if(!std::get<0>(loop_stack.top())) {
                        delete std::get<0>(range_stack.top());
                        range_stack.pop();
                    }
                    line++;
                    continue;
                }
                bool condition = true;
                int segments = tokens[line].values.size() - 1;
                if(segments == 1) {
                    condition = eval(tokens[line].values[0], globals) == "1";
                }
                else if(segments > 1) {
                    if(tokens[line].values[0] != "") {
                        if(init_loop.empty() || init_loop.top().first != depth) {
                            globals.depth--;
                            std::vector<std::string> var_names = VarTable::init_by_string(tokens[line].values[0], true, globals);
                            globals.depth++;
                            init_loop.push({ depth, var_names });
                        }
                    }
                    if(segments >= 2 && tokens[line].values[1] != "") {
                        condition = eval(tokens[line].values[1], globals) == "1";
                    }
                }
                loop_stack.push({ condition, line, depth });
                if(condition) {
                    line++;
                    continue;
                }
                else {
                    int local_depth = 1;
                    while(local_depth != 0) {
                        line++;
                        if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { local_depth++; }
                        if(tokens[line].head == "}") { local_depth--; }
                    }
                    line++;
                    depth--;
                    loop_stack.pop();
                    if(!init_loop.empty()) {
                        VarTable::gc_by_names(init_loop.top().second, globals);
                        init_loop.pop();
                    }
                    continue;
                }
            }
        }
        else if(tokens[line].head == "}") {
            VarTable::gc(globals);
            depth--;
            if(depth < 0) {
                depth = 0;
            }
            if(loop_stack.size() != 0 && (std::get<2>(loop_stack.top()) == depth + 1)) {
                std::tuple<bool, int, int> top = loop_stack.top();
                if(std::get<0>(top)) {
                    line = std::get<1>(top);
                    if((tokens[line].values[0] != "in") && (tokens[line].values.size() - 1 == 3)) {
                        VarTable::init_by_string(tokens[line].values[2], false, globals);
                    }
                }
                loop_stack.pop();
                continue;
            }
        }

        else if(ins == "continue") {
            if(loop_stack.size() != 0) {
                VarTable::gc(globals);
                std::tuple<bool, int, int> top = loop_stack.top();
                line = std::get<1>(top);
                depth = std::get<2>(top) - 1;
                if(tokens[line].values.size() - 1 == 3) {
                    VarTable::init_by_string(tokens[line].values[2], false, globals);
                }
                loop_stack.pop();
                continue;
            }
            else {
                errors::invalid_continue(globals);
            }
        }

        else if(ins == "break") {
            if(loop_stack.size() != 0) {
                int loop_depth = std::get<2>(loop_stack.top());
                while(loop_stack.size() != 0 && depth != loop_depth - 1) {
                    line++;
                    if(tokens[line].head == "}") {
                        VarTable::gc(globals);
                        depth--;
                    }
                }
                loop_stack.pop();
                if(!init_loop.empty()) {
                    VarTable::gc_by_names(init_loop.top().second, globals);
                    init_loop.pop();
                }
            }
            else {
                errors::invalid_break(globals);
            }
        }

        else if(ins == "defer") {
            int total = cmd.values.size();
            if(depth == 0) {
                // ERR:
                errors::defer_outside_fn(globals);
            }
            for(int idx = total - 1; idx >= 0; idx--) {
                defer_stack.push(std::pair<std::string, int> { cmd.values[idx], depth });
            }
        }

        else if(ins == "exit") {
            if(cmd_size == 0) {
                exit(0);
            }
            else {
                int exit_code = stoi(cmd.values[0]);
                exit(exit_code);
            }
        }

        else if(ins == "warn") {
            if(cmd_size == 0) {
                warn = !warn;
            }
            else if(cmd_size == 1) {
                if(cmd.values[0] == "off") {
                    warn = false;
                }
                else if(cmd.values[0] == "on") {
                    warn = true;
                }
            }
        }

        else if(ins == "style") {
            for(int style_itr = 0; style_itr < cmd_size; style_itr++) {
                std::string passed_style = cmd.values[style_itr];
                if(passed_style[0] == '"' && passed_style[passed_style.size() - 1] == '"') {
                    passed_style = passed_style.substr(1, passed_style.size() - 2);
                }
                std::string current_style = style::style[passed_style];

                if(current_style != "") {
                    std::cout << current_style;
                }
            }
        }

        else if(ins == "var" || ins == "static" || (ins == "" && cmd.init.size() != 0)) {
            int vars = cmd.init.size();
            bool is_static = ins == "static";
            for(int each = 0; each < vars; each += 2) {
                if(is_static && (globals.memory[cmd.init[each]] != nullptr)) {
                    continue;
                }
                VarTable::set(cmd.init[each], cmd.init[each + 1], nullptr, VAR, false, is_static ? 0 : depth, true, globals);
            }
        }

        else if(ins == "inert" && cmd.init.size() != 0) {
            int vars = cmd.init.size();
            for(int each = 0; each < vars; each += 2) {
                VarTable::set(cmd.init[each], cmd.init[each + 1], nullptr, INERT, false, depth, false, globals);
            }
        }

        else if(ins == "stdout") {
            if(cmd_size == 0) {
                std::cout << "";
                line++;
                continue;
            }

            for(int start_val = 0; start_val < cmd_size; start_val++) {
                parser::std_out(cmd.values[start_val], globals);
            }
        }

        else if(ins == "stdin") {
            EXPECT(1);
            std::cout << lib::resolve_string(cmd.values[0], globals);
            std::string input;
            std::getline(std::cin, input);
            Value* input_value = new String('"' + input + '"');
            if(cmd.target == "") {
                return input_value;
            }
            VarTable::set(cmd.target, "", input_value, VAR, true, depth, true, globals);
        }

        else if(ins == "binWrite") {
            EXPECT(2);
            BoxedValue data = get_or_make(cmd.values[0], globals);
            lib::serialize(lib::resolve_string(cmd.values[1], globals), data.value);
            data.gc();
        }

        else if(ins == "binRead") {
            EXPECT(1);
            Value* bin_value = lib::deserialize(lib::resolve_string(cmd.values[0], globals));
            if(!auto_return && cmd.target == "") {
                delete bin_value;
                line++;
                continue;
            }
            if(cmd.target == "") {
                return bin_value;
            }
            VarTable::set(cmd.target, "", bin_value, VAR, true, depth, true, globals);
        }

        else if(ins == "push") {
            int allow_insert = false;
            int insert_index = 0;
            if(cmd_size <= 1 || cmd_size > 3) {
                errors::expected_arguments(globals, ins, 2);
            }
            if(cmd.values.size() == 3) {
                allow_insert = true;
                insert_index = std::stoi(cmd.values[2]);
            }
            lib::list_push(cmd.values[0], cmd.values[1], allow_insert, insert_index, globals);
        }

        else if(ins == "len") {
            EXPECT(1);

            Value* ret_size = lib::list_len(cmd.values[0], globals);

            if(cmd.target == "") {
                return ret_size;
            }

            VarTable::set(cmd.target, "", ret_size, VAR, true, depth, true, globals);
        }

        else if(ins == "first") {
            EXPECT(1);
            Value* first = lib::list_first(cmd.values[0], globals);
            if(cmd.target == "") {
                return first;
            }

            VarTable::set(cmd.target, "", first, VAR, true, depth, true, globals);
        }

        else if(ins == "last") {
            EXPECT(1);
            Value* last = lib::list_last(cmd.values[0], globals);
            if(cmd.target == "") {
                return last;
            }

            VarTable::set(cmd.target, "", last, VAR, true, depth, true, globals);
        }

        else if(ins == "pop") {
            EXPECT(1);
            lib::list_pop(cmd.values[0], globals);
        }

        else if(ins == "popFirst") {
            EXPECT(1);
            lib::list_pop_first(cmd.values[0], globals);
        }

        else if(ins == "reverse") {
            EXPECT(1);
            lib::list_reverse(cmd.values[0], globals);
        }
        
        else if(ins == "extend") {
            EXPECT(2);
            Value* extended_list = lib::list_extend(cmd.values, globals);

            if(cmd.target == "") {
                return extended_list;
            }

            VarTable::set(cmd.target, "", extended_list, VAR, true, depth, true, globals);
        }
        
        else if(ins == "flat") {
            if(cmd_size < 1 || cmd_size > 2) {
                errors::expected_arguments(globals, ins, 2);
            }
            int level = 1;
            if(cmd.values.size() == 2) {
                level = std::stoi(cmd.values[1]);
            }

            Value* flat_list = lib::list_flat(cmd.values[0], level, globals);

            if(cmd.target == "") {
                return flat_list;
            }

            VarTable::set(cmd.target, "", flat_list, VAR, true, depth, true, globals);
        }

        else if(ins == "keys") {
            EXPECT(1);
            Value* keys = lib::dict_keys(cmd.values[0], globals);
            if(cmd.target == "") {
                return keys;
            }
            VarTable::set(cmd.target, "", keys, VAR, true, depth, true, globals);
        }

        else if(ins == "values") {
            EXPECT(1);
            Value* values = lib::dict_values(cmd.values[0], globals);
            if(cmd.target == "") {
                return values;
            }
            VarTable::set(cmd.target, "", values, VAR, true, depth, true, globals);
        }

        else if(ins == "items") {
            EXPECT(1);
            Value* items = lib::dict_items(cmd.values[0], globals);
            if(cmd.target == "") {
                return items;
            }
            VarTable::set(cmd.target, "", items, VAR, true, depth, true, globals);
        }

        else if(ins == "update") {
            EXPECT(2);
            Value* updated_dict = lib::dict_update(cmd.values[0], cmd.values[1], globals);
            if(cmd.target == "") {
                return updated_dict;
            }
            VarTable::set(cmd.target, "", updated_dict, VAR, true, depth, true, globals);
        }

        else if(ins == "exists") {
            EXPECT(2);
            Value* exists = lib::dict_key_exists(cmd.values[0], cmd.values[1], globals);
            if(cmd.target == "") {
                return exists;
            }
            VarTable::set(cmd.target, "", exists, VAR, true, depth, true, globals);
        }

        else if(ins == "type") {
            EXPECT(1);
            std::string type_str = get_type(eval(cmd.values[0], globals), globals);
            Value* type = new String('"' + type_str + '"');
            if(cmd.target == "") {
                return type;
            }

            VarTable::set(cmd.target, "", type, VAR, true, depth, true, globals);
        }

        if(cmd_values_modified) {
            cmd.values = current_cmd_values;
        }

        line++;
    }

    if(fn_defer) {
        exec_defer(globals);
    }

    std::cout << style::style["reset"];

    return nullptr;
}
