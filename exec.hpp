#pragma once

#include "config.hpp"
#include "lexer.hpp"
#include "errors.hpp"
#include "expr_parser.hpp"
#include "var.hpp"
#include "lib/lib_style.hpp"
#include "lib/lib_string.hpp"

#include <stack>
#include <utility>
#include <tuple>
//#include "lib/lib_list.hpp"

namespace parser {
    void std_out(std::string out_text) {
        if(out_text[0] == '$') {
            std::cout << lib::resolve_string(VarTable::print(out_text));
            return;
        }
        else if(out_text[0] == '(') {
            std::cout << lib::resolve_string(eval(out_text));
            return;
        }
        std::cout << lib::resolve_string(lib::render_escape_chars(out_text));
    }

    void std_err(std::string err_text) {
        std::cerr << lib::resolve_string(lib::render_escape_chars(err_text));
    }
}

int depth = 0;
std::stack<std::string> call_stack;

Value* line_exec(std::vector<Token>& tokens, bool auto_return = false) {
    bool warn = true;
    int total_tokens = tokens.size();

    int line = 0;
    //int depth = 0;
    int current_depth = 0;
    std::stack<std::pair<bool, int>> conditional_stack;
    std::stack<std::tuple<bool, int, int>> loop_stack;

    while(line < total_tokens) {
        //std::cout << "Line: " << (line + 1) << " Token: " << tokens[line].head << " Size: " << tokens[line].values.size() << "\n";
        Token& cmd = tokens[line];
        std::string& ins = cmd.head;
        int cmd_size = cmd.values.size();
        if(ins[0] == '#' && ins[1] == '!') {
            line++;
            continue;
        }

        if(cmd.head == "<-") {
            std::string result = eval(cmd.target);
            if(result[0] == '$') {
                //result = VarTable::print(result);
                //return copy(VarTable::get(result, {}, true, true));
                return copy(VarTable::get(result, {}, true, true));
            }
            // TODO: send this result value to the outer scope and assign it to the target variable.
            //return new Value();
            return make_value(result);
        }

        if(cmd.head[0] == ':') {
            std::string fn_name = cmd.head.substr(1);
            Fn* fn = Functions::fn[fn_name];
            int args_size = cmd.values.size();

            depth++;
            int arg;
            for(arg = 0; arg < args_size; arg++) {
                std::string& r_val = cmd.values[arg];
                // perhaps eval here is not needed.
                /*if(r_val[0] == '(' || (r_val[0] == '$' && r_val[1] == '(')) {
                    //std::cout << "R_Val: " << r_val << "\n";
                    r_val = eval(r_val);
                    //std::cout << "R_Val evaled: " << r_val << "\n";
                }*/
                // std::cout << fn->init[arg * 2] << ": " << r_val << " (" << depth <<")" << "\n";
                VarTable::set(fn->init[arg * 2], r_val, nullptr, VAR, false, depth);
            }

            int all = (fn->init.size() / 2);
            for(int rest = arg; rest < all; rest++) {
                std::string& r_val = fn->init[(rest * 2) + 1];
                if(r_val[0] == '(') {
                    r_val = eval(r_val);
                }
                VarTable::set(fn->init[rest * 2], r_val, nullptr, VAR, false, depth);
                //std::cout << fn->init[2*rest] << "(" << (2*rest) << ")" << " : " << fn->init[2*rest + 1] << "(" << (2*rest + 1) << ")" << "\n";
            }

            call_stack.push(fn_name);
            Value* return_value = line_exec(fn->body);
            if(return_value != nullptr && !auto_return) {
                VarTable::set(cmd.target, "", return_value, VAR, true, depth - 1);
            }
            //delete return_value;
            VarTable::gc(depth);
            depth--;
            call_stack.pop();
            if(auto_return) {
                return return_value;
            }
        }

        if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") {
            // maybe incr and set these values in the if-blocks;
            depth += 1;
            current_depth = depth;
            //std::cout << tokens[line].values[0] << " : " << depth << "\n";
            if(tokens[line].head == "if") {
                // might need to refactor values into a variable.
                bool condition = eval(tokens[line].values[0]) == "1";
                conditional_stack.push({ condition, depth });
                //std::cout << "Stack: " << condition << " " << depth << "\n";
                if(conditional_stack.top().first && tokens[line].head != "else") {
                    line++;
                    continue;
                }
                else {
                    //std::cout << "If End: " << depth << " " << (current_depth - 1) << "\n";
                    while(depth != current_depth - 1) {
                        line++;
                        if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                        if(tokens[line].head == "}") { depth--; }
                    }
                    //std::cout << "end if: " << line << "\n";
                }
            }
            else if(tokens[line].head == "else") {
                //std::cout << "here " << depth << "\n";
                std::pair<bool, int> check = conditional_stack.top();
                //std::cout << "Stack: " << check.first << " " << check.second << " | Depth: " << depth << "\n";
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
                        //continue;
                    }
                }
                else {
                    while(depth != current_depth - 1) {
                        line++;
                        if(tokens[line].values.size() != 0 && tokens[line].values[tokens[line].values.size() - 1] == "{") { depth++; }
                        if(tokens[line].head == "}") { depth--; }
                    }
                    //continue;
                }
            }
            else if(tokens[line].head == "loop") {
                bool condition = eval(tokens[line].values[0]) == "1";
                loop_stack.push({ condition, line, depth });
                //std::cout << "Loop Depth: " << depth << " " << (line + 1) << "\n";
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
                    continue;
                }
            }
        }
        else if(tokens[line].head == "}") {
            VarTable::gc(depth);
            depth--;
            if(depth < 0) {
                depth = 0;
            }
            //std::cout << "Line: " << (line + 1) << " Actual: " << std::get<2>(loop_stack.top()) << " Required: " << (depth + 1) << "\n";
            /// temporary solution for the problem stated in FIGURED.
            /*int new_depth = depth + 1;
            if(loop_stack.size() == 1) {
                new_depth = depth;
            }*/
            ///
            // FIGURED: the depths do not match for the last iteration (when stack size is 1). difference of 1.
            if(loop_stack.size() != 0 && (std::get<2>(loop_stack.top()) == depth + 1 /*new_depth*/)) {
                std::tuple<bool, int, int> top = loop_stack.top();
                if(std::get<0>(top)) {
                    line = std::get<1>(top);
                }
                loop_stack.pop();
                continue;
            }
        }

        else if(ins == "continue") {
            if(loop_stack.size() != 0) {
                //std::cout << "Line: " << line << "\n";
                //std::cout << "Size: " << loop_stack.size() << "\n";
                //std::cout << "[" << std::get<0>(loop_stack.top()) << " " << std::get<1>(loop_stack.top()) << " " << std::get<2>(loop_stack.top()) << "]\n";
                VarTable::gc(depth);
                std::tuple<bool, int, int> top = loop_stack.top();
                line = std::get<1>(top);
                depth = std::get<2>(top) - 1;
                //depth--;
                //std::cout << "Line: " << line << "\n";
                loop_stack.pop();
                continue;
            }
        }

        else if(ins == "break") {
            if(loop_stack.size() != 0) {
                int loop_depth = std::get<2>(loop_stack.top());
                // std::cout << "[" << depth << "]\n";
                // std::cout << "[" << loop_depth << "]" << "\n";
                while(loop_stack.size() != 0 && loop_depth != depth + 1) {
                    line++;
                    if(tokens[line].head == "}") {
                        depth--;
                    }
                }
                loop_stack.pop();
                continue;
                // std::cout << "Line: " << line << "\n";
                // std::cout << "Size: " << loop_stack.size() << "\n";
            }
        }

        else if(ins == "exit") {
            if(cmd_size == 0) {
                exit(0);
            }
            else {
                int exit_code = stoi(cmd.values[1]);
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
                VarTable::set(cmd.init[each], cmd.init[each + 1], nullptr, VAR, false, is_static ? 0 : depth);
            }
        }

        else if(ins == "inert" && cmd.init.size() != 0) {
            int vars = cmd.init.size();
            for(int each = 0; each < vars; each += 2) {
                VarTable::set(cmd.init[each], cmd.init[each + 1], nullptr, INERT, false, depth);
            }
        }

        else if(ins == "stdout") {
            if(cmd_size == 0) {
                std::cout << "";
                line++;
                continue;
            }

            for(int start_val = 0; start_val < cmd_size; start_val++) {
                parser::std_out(cmd.values[start_val]);
            }
        }

        /*else if(ins == "stdin" && cmd_size == 1) {
            std::string var_to_read = lexer::get_var_name_from_token(cmd.values[0]);
            if(var.get_mem_type(var_to_read) == "const") {
                errors::change_const_var_error(var_to_read);
            }
            //var.read_var(var.expand_var(cmd[1]));
            var.read_var(cmd.values[0]);
        }*/

        line++;
    }

    std::cout << style::style["reset"];

    return nullptr;
}