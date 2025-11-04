#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <sstream>
#include <algorithm>

#include "globals.hpp"
#include "lexer.hpp"
#include "types.hpp"
#include "expr_parser.hpp"
#include "errors.hpp"
#include "lib/lib_string.hpp"
#include "lib/lib_style.hpp"

#define TO_NUM(value)  (dynamic_cast<Number*>(value))
#define TO_STR(value)  (dynamic_cast<String*>(value))
#define TO_LIST(value) (dynamic_cast<List*>(value))
#define TO_DICT(value) (dynamic_cast<Dict*>(value))
#define TO_NULL(value) (dynamic_cast<Null*>(value))
#define TO_REF(value)  (dynamic_cast<Ref*>(value))

std::vector<std::string> parse_map(std::string text, int& index) {
    index++;
    std::string contents = parser::extract_list(text, '(', index);
    contents = contents.substr(1, contents.size() - 2);

    int pos = 0;
    std::vector<std::string> vals = parser::parse_init(contents, pos, "->");
    return vals;
}

Value* copy(Value*);
std::unordered_map<std::string, Value*> memory;
namespace VarTable {
    Value* get(std::string, std::vector<std::string> = {}, bool = false, bool = false, bool = true);
}

Value* make_value(std::string value) {
    Value* val = nullptr;
    if((value[0] >= '0' && value[0] <= '9') || value[0] == '-') {
        val = new Number(value);
    }
    else if(value[0] == '"') {
        val = new String(value.c_str());
    }
    else if(value[0] == '[') {
        val = new List(value);
    }
    else if(value[0] == '#' && value[1] == '(') {
        val = new Dict(value);
    }
    else if(value == "null") {
        val = new Null();
    }
    else if(/*value[0] == '$'*/ parser::is_var(value)) {
        val = VarTable::get(value, {});
    }
    return val;
}

///
Ref::Ref(Value* val, Value* p) : ref(val), parent(p) {}
std::string Ref::print() {
    return ref->print();
}
Ref::~Ref() {}
///

/// Null
Null::Null() {}
std::string Null::print() {
    return "null";
}
Null::~Null() {}
///

/// Number
// maybe remove that eval.
// eval will be done at the parser level.
Number::Number(std::string Val) : val(Val) {}
std::string Number::print() {
    return val;
}
Number::~Number() {}
///

/// String
String::String(std::string Str) {
    int len = Str.size();
    str = new char[len + 1];
    strcpy(str, Str.c_str());
    str[len] = '\0';
}

std::string String::print() {
    return std::string(str);
}

std::string String::val() {
    return lib::resolve_string(std::string(str));
}

String::~String() {
    delete[] str;
    str = nullptr;
}
///

/// List

List::List() {}

List::List(std::string list) {
    int index = 0;
    std::vector<std::string> values = parser::parse_list(list, index);
    for(std::string v : values) {
        //v = eval(v);
        /*if(v[0] >= '0' && v[0] <= '9') {
            items.emplace_back(new Number(v));
        }
        else if(v[0] == '"') {
            items.emplace_back(new String(v.c_str()));
        }
        else if(v[0] == '[') {
            items.emplace_back(new List(v));
        }
        else if(v[0] == '#' && v[1] == '(') {
            items.emplace_back(new Dict(v));
        }
        else if(v == "null") {
            items.emplace_back(new Null());
        }
        else if(v[0] == '$') {
            items.emplace_back(VarTable::get(v, {}));
        }*/
        items.emplace_back(make_value(eval(v)));
        // 1st make_value
    }
}

std::string List::print() {
    int size = items.size();
    int last = size - 1;
    std::string sep = ", ";
    std::stringstream disp;
    disp << "[";
    for(int v = 0; v < size; v++) {
        if(items[v]->type == "Number") {
             disp << ((Number*)items[v])->print();
        }
        else if(items[v]->type == "String") {
             disp << ((String*)items[v])->print();
        }
        else if(items[v]->type == "Dict") {
             disp << ((Dict*)items[v])->print();
        }
        else if(items[v]->type == "Null") {
            disp << ((Null*)items[v])->print();
        }
        else {
             disp << items[v]->print();
        }

        if(v == last) {
            sep = "";
        }
        disp << sep;
     }
     disp << "]";

     return disp.str();
}

List::~List() {
    for(Value*& v : items) {
        delete v;
        v = nullptr;
    }
}
///

/// Dict
Dict::Dict() {}

Dict::Dict(std::string dict_val) {
    int index = 0;
    std::vector<std::string> kv = parse_map(dict_val, index);
    int size = kv.size();
    keys.reserve(size / 2);
    for(int i = 0; i < size; i += 2) {
        // if the key already exists, then free it's value first.
        if(kv[i][0] == '"' && kv[i][kv[i].size() - 1] == '"') {
            kv[i] = lib::resolve_string(kv[i]);
        }
        append_unique(kv[i], true);
        //kv[i + 1] = eval(kv[i + 1]);
        /*if(kv[i + 1][0] >= '0' && kv[i + 1][0] <= '9') {
            dict[kv[i]] = new Number((kv[i + 1]));
        }
        else if(kv[i + 1][0] == '"') {
            dict[kv[i]] = new String(kv[i + 1].c_str());
        }
        else if(kv[i + 1][0] == '[') {
            dict[kv[i]] = new List(kv[i + 1]);
        }
        else if(kv[i + 1][0] == '#' && kv[i + 1][1] == '(') {
            dict[kv[i]] = new Dict(kv[i + 1]);
        }
        else if(kv[i + 1] == "null") {
            dict[kv[i]] = new Null();
        }
        else if(kv[i + 1][0] == '$') {
            dict[kv[i]] = VarTable::get(kv[i + 1], {});
        }*/
        dict[kv[i]] = make_value(eval(kv[i + 1]));
    }
}

void Dict::append_unique(std::string key, bool del_val) {
    if(std::find(keys.begin(), keys.end(), key) == keys.end()) {
        keys.emplace_back(key);
    }
    else if(del_val) {
        delete dict[key];
        dict[key] = nullptr;
    }
}

std::string Dict::print() {
    std::stringstream disp;
    std::string last = keys[keys.size() - 1];
    std::string sep = ", ";
    disp << "#(";
    for(std::string key : keys) {
        Value*& value = dict[key];
        disp << key << " -> ";
        /*if(value->type == "Number") {
            disp << ((Number*)value)->print();
        }*/
        if(TO_NUM(value)) {
            disp << TO_NUM(value)->print();
        }
        else if(TO_STR(value)) {
            disp << TO_STR(value)->print();
        }
        else if(TO_LIST(value)) {
            disp << TO_LIST(value)->print();
        }
        else if(TO_DICT(value)) {
            disp << TO_DICT(value)->print();
        }
        else if(TO_NULL(value)) {
            disp << TO_NULL(value)->print();
        }
        else if(TO_REF(value)) {
            disp << TO_REF(value)->print();
        }
        else {
            std::cout << "invalid pointer\n";
            exit(0);
        }

        if(last == key) {
            sep = "";
        }

        disp << sep;
    }
    disp << ")";

    return disp.str();
}

Dict::~Dict() {
    std::unordered_map<std::string, Value*>::iterator itr;
    for(itr = dict.begin(); itr != dict.end(); itr++) {
        if(itr->second != nullptr) {
            delete itr->second;
            dict[itr->first] = nullptr;
        }
    }
}
///

Value* copy(Value* value) {
    Value* duplicate = nullptr;

    if(TO_NUM(value)) {
        duplicate = new Number(TO_NUM(value)->val);
    }
    else if(TO_STR(value)) {
        duplicate = new String(TO_STR(value)->str);
    }
    else if(TO_NULL(value)) {
        duplicate = new Null();
    }
    else if(TO_LIST(value)) {
        duplicate = new List();
        List* list_val = TO_LIST(value);
        List* list_dup = TO_LIST(duplicate);
        list_dup->items.reserve(list_val->items.size());
        for(Value* v : list_val->items) {
            list_dup->items.emplace_back(copy(v));
        }
    }
    else if(TO_DICT(value)) {
        duplicate = new Dict();
        Dict* dict_val = TO_DICT(value);
        Dict* dict_dup = TO_DICT(duplicate);
        dict_dup->keys.reserve(dict_val->keys.size());
        for(std::string& key : dict_val->keys) {
            dict_dup->keys.emplace_back(key);
        }
        for(std::pair<std::string, Value*> key_val : dict_val->dict) {
            dict_dup->dict[key_val.first] = copy(key_val.second);
        }
    }
    else if(TO_REF(value)) {
        // check it it's necessary to copy the parent pointer too.
        duplicate = new Ref(TO_REF(value)->ref);
    }

    if(duplicate == nullptr) { std::cout << "cannot determine type\n"; }

    return duplicate;
}

namespace RefTable {
    //std::vector<Value*> variables;
    std::unordered_map<Value*, uint64_t> variables;
    std::vector<std::vector<Ref*>> references;

    void add(Value*& val) {
        /*std::vector<Value*>::iterator idx_itr = find(variables.begin(), variables.end(), val);
        if(idx_itr == variables.end()) {
            variables.emplace_back(val);
            references.emplace_back(std::vector<Ref*> {});
        }*/
        bool exists = variables[val] != 0;
        if(!exists) {
            variables[val] = references.size() + 1;
            // std::cout << val << " : " << variables[val] << "\n";
            references.emplace_back(std::vector<Ref*> {});
        }
    }

    void add_ref(Value*& var, Ref* var_ref) {
        /*std::vector<Value*>::iterator idx_itr = find(variables.begin(), variables.end(), var);
        if(idx_itr != variables.end()) {
            int idx = idx_itr - variables.begin();
            references[idx].emplace_back(var_ref);
        }*/
        // std::cout << variables.size() << " " << references.size() << "\n";
        bool exists = variables[var] != 0;
        if(exists) {
            int idx = variables[var] - 1;
            references[idx].emplace_back(var_ref);
        }
    }

    void change_var(Value* old_var, Value* new_var) {
        /*std::vector<Value*>::iterator idx_itr = find(variables.begin(), variables.end(), old_var);
        if(idx_itr != variables.end()) {
            int idx = idx_itr - variables.begin();
            // delete variables[idx];
            variables[idx] = new_var;
            for(Ref*& r : references[idx]) {
                r->ref = new_var;
            }
        }*/
        bool exists = variables[old_var] != 0;
        if(exists) {
            int idx = variables[old_var];
            variables.erase(old_var);
            variables[new_var] = idx;
            for(Ref*& r : references[idx - 1]) {
                r->ref = new_var;
            }
        }
    }
}

namespace InertTable {
    std::unordered_map<std::string, std::string> vars = {};
    std::unordered_map<std::string, bool> is_hit = {};
};

namespace ScopeTable {
    std::unordered_map<std::string, int> scope = {};
}

namespace VarTable {
    //void set(std::string, std::string, Value* data_ptr = nullptr, Type type = VAR, bool disallow_copy = false, int depth = 0, bool allow_shadowing = false);

    void gc_value(std::string name, Value* val) {
        if(val != nullptr) {
            if(val->shadow != nullptr && val->shadow->size() != 0) {
                std::pair<Value*, int> top = val->shadow->top();
                if(top.second >= depth) {
                    delete top.first;
                    val->shadow->pop();
                    //std::cout << "Shadow Stack Size: " << val->shadow->size();
                    if(val->shadow->size() == 0) {
                        val->gc_shadow();
                    }
                }
            }
            else if(ScopeTable::scope[name] >= depth) {
                delete val;
                memory[name] = nullptr;
            }
        }
    }

    void gc(int depth = 0) {
        std::unordered_map<std::string, Value*>::iterator itr, end = memory.end();
        for(itr = memory.begin(); itr != end; itr++) {
            gc_value(itr->first, itr->second);
        }
        if(depth == 0) {
            Functions::gc();
        }
    }

    void gc_by_names(std::vector<std::string>& var_names) {
        for(std::string& name : var_names) {
            gc_value(name, memory[name]);
        }
    }

    Value* get(std::string name, std::vector<std::string> symbols, bool update, bool for_print, bool get_original) {
        // eval the inert var when it's used.
        if(name[0] >= '0' && name[0] <= '9') {
            return nullptr;
        }
        if(name != "" && InertTable::vars[name/*.substr(1)*/] != "" && !InertTable::is_hit[name/*.substr(1)*/]) {
            std::string Name = name/*.substr(1)*/;
            //std::cout << "adding: " << Name << " value: " << InertTable::vars[Name] << std::endl;
            set(Name, InertTable::vars[Name]);
            InertTable::is_hit[Name] = true;
        }
        //

        if(name != "" && memory[name/*.substr(1)*/] != nullptr) {
            Value* fetched_var = memory[name/*.substr(1)*/];
            Value* ret_var = nullptr;
            if(fetched_var != nullptr && fetched_var->shadow != nullptr && fetched_var->shadow->size() != 0) {
                ret_var = fetched_var->shadow->top().first;
                // ret_var is nullptr in this case.

                //std::cout << "stack size: " << fetched_var->shadow.size() << " " << ret_var << "\n";
                //fetched_var->shadow.pop();
                //std::cout << "ret_var:" << ret_var << "\n";
                ///return copy(ret_var);
                //if(name.substr(1) == "n") { std::cout << "n real val: " << ret_var->print() << "\n"; }
                //std::cout << "Ret Var: " << ret_var << "\n";
                return (for_print) ? ret_var : copy(ret_var);
            } 
        }

        if(symbols.size() == 0) {
            symbols = get_var_with_indices(name);
        }

        std::string var_name = symbols[0];
        /*if(var_name[0] != '$') {
            std::cout << "expected $\n";
            exit(0);
        }*/
        bool is_ref = false;
        //uint8_t pos = /*1*/ 0;
        if(var_name[/*1*/ 0] == '&') {
            is_ref = true;
            var_name = var_name.substr(1);
            //pos++;
        }
        //var_name = var_name.substr(pos);
        Value* var = memory[var_name];
        int size = symbols.size();
        int bound = size;

        //bool at_parent = false;
        if(var == nullptr) {
            //errors::undefined_var(call_stack, name, name);
            return var;
        }
        Value* p = nullptr;
        if(size > 1) {
            if(update && !for_print) {
                bound--;
                //at_parent = true;
            }
            for(int i = 1; i < bound; i++) {
                if(TO_REF(var)) {
                    var = TO_REF(var)->ref;
                }
                if(symbols[i][0] >= '0' && symbols[i][0] <= '9') {
                    int index = std::stoi(symbols[i]);
                    int size = TO_LIST(var)->items.size();
                    if(index >= size) {
                        errors::index_error(call_stack, name, symbols[i]);
                    }
                    var = TO_LIST(var)->items[index];
                }
                else if(symbols[i][0] == '"' && symbols[i][symbols[i].size() - 1] == '"') {
                    std::string key = lib::resolve_string(symbols[i]);
                    var = TO_DICT(var)->dict[key];
                    if(var == nullptr) {
                        errors::key_error(call_stack, name, key);
                    }
                }
                if(!update && bound == 2) {
                    p = memory[var_name];
                }
                // make experiments to check if it should be -1 or -2.
                else if(!update && i == size - 2) {
                    p = var;
                }
                /*if(at_parent) {
                    p = var;
                }*/
                /*else {
                    if(i == size - 2) {
                        std::cout << "p: " << var << "\n";
                        p = var;
                    }
                }*/
            }
        }

        if(for_print) {
            return var;
        }

        if(get_original && TO_REF(var)) {
            var = TO_REF(var)->ref;
        }

        if(!is_ref && !update) {
            var = copy(var);
        }
        if(is_ref) {
            RefTable::add(var);
            Value* tmp = var;
            var = new Ref(var, p);
            RefTable::add_ref(tmp, TO_REF(var));
        }

        return var;
    }

    void unpack(std::string list, std::string structure, Value* data_ptr = nullptr) {
        int index = 0;
        Value* packed_items = nullptr;
        bool is_literal = false;
        // parse the list like dict here.
        //std::vector<std::string> items;
        if(list[0] == '#' && list[1] == '(') {
            int end = list.size();
            //list[0] = ' ';
            list[1] = '[';
            list[end - 1] = ']';
            index++;
        }
        //std::cout << "literal: " << list << std::endl;
        std::vector<std::string> items = parser::parse_list(list, index);
        //
        uint64_t len = items.size();
        if(structure != "" && data_ptr == nullptr) {
            if(/*structure[0] == '$'*/ parser::is_var(structure)) {
                packed_items = get(structure, {}, true, true);
            }
            else {
                if(structure[0] == '[') {
                    packed_items = new List(structure);
                }
                else if(structure[0] == '#' && structure[1] == '(') {
                    packed_items = new Dict(structure);
                }
                is_literal = true;
            }
        }
        else {
            packed_items = data_ptr;
        }

        if(TO_LIST(packed_items)) {
            if(len > (TO_LIST(packed_items))->items.size()) {
                // add proper error msg here.
                std::cerr << "more than enough items to unpack" << std::endl;
                if(is_literal) {
                    delete packed_items;
                }
                return;
            }

            for(uint64_t i = 0; i < len; i++) {
                if(items[i] == "_") {
                    continue;
                }
                if(items[i][0] == '[' || (items[i][0] == '#' && items[i][1] == '(')) {
                    unpack(items[i], "", TO_LIST(packed_items)->items[i]);
                }
                else {
                    set(items[i], "", TO_LIST(packed_items)->items[i]);
                }
            }
        }
        else if(TO_DICT(packed_items)) {
            if(len > TO_DICT(packed_items)->keys.size()) {
                std::cerr << "more than enough items to unpack" << std::endl;
                if(is_literal) {
                    delete packed_items;
                }
                return;
            }
            for(uint64_t j = 0; j < len; j++) {
                if(TO_DICT(packed_items)->dict[items[j]] == nullptr) {
                    std::cerr << "key \"" << items[j] << "\" does not exits." << std::endl;
                    if(is_literal) {
                        delete packed_items;
                    }
                    return;
                }
                set(items[j], "", TO_DICT(packed_items)->dict[items[j]]);
            }
        }

        if(is_literal) {
            delete packed_items;
        }
    }

    void set(std::string var, std::string data, Value* data_ptr, Type type, bool disallow_copy, int depth, bool allow_shadowing) {
        //std::cout << "raw: " << data << std::endl;
        //if(allow_shadowing)
        //    std::cout << "Setting " << var << " to " << data << "\n";
        //else
        //    std::cout << "Re-assigning " << var << " to " << data << "\n";*/

        bool is_shadowed = false;
        if(var[0] == '[' || (var[0] == '#' && var[1] == '(')) {
            unpack(var, data);
            return;
        }
        if(type == INERT) {
            //std::cout << "to_inert: " << var << " = " << data << "\n";
            InertTable::vars[var] = data;
            InertTable::is_hit[var] = false;
            return;
            // Need to modify eval to get var reading complete.
        }
        if(data != "") {
            data = eval(data);
        }

        if(memory[var] == nullptr) {
            ScopeTable::scope[var] = depth;
        }


        //std::cout << "eval: " << data << std::endl;
        Value* ptr = VarTable::get(var/*.substr(1)*/, {}, true, true);
        if(/*var[0] == '$'*/ ptr != nullptr && !allow_shadowing && parser::is_var(var)) {
            // TODO: the same for Strings. (DONE)
            // TODO: the impl is done for literals, add resolve code for vars and refs. (DONE)
            //std::cout << "Mem Value = " << ptr << "\n";
            if(data != "" && /*data[0] == '$'*/ parser::is_var(data)) {
                Value* d_ptr = VarTable::get(data, {}, true, true);
                if(TO_REF(d_ptr)) {
                    d_ptr = TO_REF(d_ptr)->ref;
                }
                if(TO_NUM(d_ptr)) {
                    data = TO_NUM(d_ptr)->val;
                }
                else if(TO_STR(d_ptr)) {
                    data = std::string(TO_STR(d_ptr)->str);
                }
            }
            if(TO_REF(ptr)) {
                ptr = TO_REF(ptr)->ref;
            }
            if(TO_NUM(ptr) && data != "") {
                // check if the second value is also of the same type. (tbd)
                TO_NUM(ptr)->val = data; //(dynamic_cast<Number*>(value))->val;
                //delete value;
                return;
            }
            else if(TO_STR(ptr) && data != "") {
                strcpy(TO_STR(ptr)->str, data.c_str());
                return;
            }
        }
        if(var[0] == '&') {
            std::cout << "Cannot use `&` while setting a variable.\n";
            exit(0);
        }
        

        Value* value = nullptr;
        if(data == "" && data_ptr != nullptr) {
            value = disallow_copy ? data_ptr : copy(data_ptr);
        }

        /*else if(data[0] >= '0' && data[0] <= '9') {
            value = new Number(data);
        }
        else if(data[0] == '"') {
            value = new String(data);
        }
        else if(data[0] == '[') {
            value = new List(data);
        }
        else if(data[0] == '#' && data[1] == '(') {
            value = new Dict(data);
        }
        else if(data == "null") {
            value = new Null();
        }*/
        else if(/*data[0] == '$'*/ parser::is_var(data)) {
            value = get(data, {});
            if(TO_REF(value)) {
                RefTable::add(TO_REF(value)->ref);
                RefTable::add_ref((TO_REF(value)->ref), TO_REF(value));
            }
        }
        else {
            //std::cout << "Shadowing Value 1: " << value << "\n";
            //std::cout << "Data: " << data << "\n";
            value = make_value(data);
            //std::cout << "Shadowing Value 2: " << value << "\n";
        }

        if(allow_shadowing && memory[var] != nullptr /*&& var[0] != '$'*/) {
            if(memory[var]->shadow == nullptr) {
                memory[var]->init_shadow();
            }
            memory[var]->shadow->push({ value, depth });
            is_shadowed = true;
            //return;
        }

        if(/*var[0] == '$'*/ ptr != nullptr && !allow_shadowing && parser::is_var(var)) {
            //
            //
            std::string v_name = var/*.substr(1)*/;
            if(memory[v_name] != nullptr) {
                if(TO_REF(memory[v_name])) {
                    std::unordered_map<std::string, Value*>::iterator itr;
                    Value* oval = TO_REF(memory[v_name])->ref;
                    std::string ovar = "";
                    bool found_ovar = false;
                    for(itr = memory.begin(); itr != memory.end(); itr++) {
                        if(itr->second == oval) {
                            ovar = itr->first;
                            found_ovar = true;
                            break;
                        }
                    }
                    if(found_ovar) {
                        Value* prev = TO_REF(memory[v_name])->ref;
                        RefTable::change_var(prev, value);
                        delete prev;
                        memory[ovar] = value;
                    }
                    else {
                        Value* prev_val = TO_REF(memory[v_name])->ref;
                        Value* parent_val = TO_REF(memory[v_name])->parent;
                        Value* prev =  TO_REF(memory[v_name])->ref;
                        RefTable::change_var(prev, value);
                        delete prev;
                        if(dynamic_cast<List*>(parent_val)) {
                            List* p_list = TO_LIST(parent_val);
                            for(uint64_t i = 0; i < p_list->items.size(); i++) {
                                if(prev_val == p_list->items[i]) {
                                    p_list->items[i] = value;
                                    break;
                                }
                            }
                        }
                        // TODO: perform the same operation for dict as done for list above. (done)
                        else if(TO_DICT(parent_val)) {
                            Dict* p_dict = TO_DICT(parent_val);
                            std::unordered_map<std::string, Value*>::iterator itr;
                            for(itr = p_dict->dict.begin(); itr != p_dict->dict.end(); itr++) {
                                if(prev_val == itr->second) {
                                    // might have to use append_unique() here.
                                    p_dict->dict[itr->first] = value;
                                    break;
                                }
                            }
                        }
                    }
                }
                else {
                    Value* prev = memory[v_name];
                    RefTable::change_var(prev, value);
                    delete prev;
                    memory[v_name] = value;
                }
            }
            else {
                std::vector<std::string> syms = get_var_with_indices(expand_var(var));
                std::string last_symbol = syms[syms.size() - 1];
                Value* v = get("", syms, true);


                // get the orignal val and check if it's a ref. if yes, then use the upper and below method to change it.
                Value* ov = get("", syms, false, false, false);
                //std::cout << "---> " << ov->print() << std::endl;
                bool is_ref = false;
                if(TO_REF(ov)) {
                    is_ref = true;
                }
                delete ov;

                if(TO_LIST(v)) {
                    ////Value* r = new Ref(value); //
                    if(is_ref) {
                        //RefTable::change_var((dynamic_cast<Ref*>(ov))->ref, value);
                        Value* tmp = TO_REF((TO_LIST(v)->items[std::stoi(last_symbol)]))->ref;
                        //
                        bool found = false;
                        std::unordered_map<std::string, Value*>::iterator itr;
                        for(itr = memory.begin(); itr != memory.end(); itr++) {
                            if(itr->second == tmp) {
                                memory[itr->first] = value;
                                found = true;
                                break;
                            }
                        }
                        if(!found) {
                            Value* tmp_parent = TO_REF((TO_LIST(v)->items[std::stoi(last_symbol)]))->parent;
                            // NEW TODO: Might need to implement the below logic for Dict and wrap the overall code in a different function.
                            if(dynamic_cast<List*>(tmp_parent)) {
                                List* tmp_pd = TO_LIST(tmp_parent);
                                for(uint64_t x = 0; x < tmp_pd->items.size(); x++) {
                                    if(tmp_pd->items[x] == tmp) {
                                        tmp_pd->items[x] = value;
                                        break;
                                    }
                                }
                            }
                        }
                        //
                        TO_REF((TO_LIST(v)->items[std::stoi(last_symbol)]))->ref = value;
                        //RefTable::change_var( (dynamic_cast<Ref*>((dynamic_cast<List*>(v))->items[std::stoi(last_symbol)]))->ref, value );
                        RefTable::change_var(tmp, value);
                        delete tmp;
                    }
                    else {
                        Value* prev_val = TO_LIST(v)->items[std::stoi(last_symbol)];
                        //delete (dynamic_cast<List*>(v))->items[std::stoi(last_symbol)];
                        TO_LIST(v)->items[std::stoi(last_symbol)] = value;
                        RefTable::change_var(prev_val, value);
                        // maybe this line should not exist, but it also prevents the memory leak from happening... investigate more.
                        // this line cause segfaults (as it should for other cases.)
                        delete prev_val;
                    }
                }
                else if(TO_DICT(v)) {
                    // TODO: Write code for replacing when item is Ref as done above in List.
                    if(is_ref) {
                        Value* tmp = TO_REF(TO_DICT(v)->dict[last_symbol])->ref;
                        bool found = false;
                        std::unordered_map<std::string, Value*>::iterator itr;
                        for(itr = memory.begin(); itr != memory.end(); itr++) {
                            if(itr->second == tmp) {
                                found = true;
                                break;
                            }
                        }
                        if(!found) {
                            Value* tmp_parent = TO_REF(TO_DICT(v)->dict[last_symbol])->parent;
                            // NEW TODO: Might need to implement the below logic for Dict and wrap the overall code in a different function.
                            if(TO_LIST(tmp_parent)) {
                                List* tmp_pd = TO_LIST(tmp_parent);
                                for(uint64_t x = 0; x < tmp_pd->items.size(); x++) {
                                    if(tmp_pd->items[x] == tmp) {
                                        tmp_pd->items[x] = value;
                                        break;
                                    }
                                }
                            }
                        }
                        TO_REF(TO_DICT(v)->dict[last_symbol])->ref = value;
                        RefTable::change_var(tmp, value);
                        delete tmp;
                    }
                    else {
                        // add change_ref() logic here.
                        last_symbol = lib::resolve_string(last_symbol);
                        Value* old = TO_DICT(v)->dict[last_symbol];
                        RefTable::change_var(old, value);
                        delete old;
                        TO_DICT(v)->dict[last_symbol] = value;
                        TO_DICT(v)->append_unique(last_symbol);
                    }
                }
            }
            ///
            RefTable::add(value);
            if(TO_REF(value)) {
                RefTable::add(TO_REF(value)->ref);
                RefTable::add_ref(TO_REF(value)->ref, TO_REF(value));
            }
            ///
        }
        else {
            if(!is_shadowed) {
                if(memory[var] != nullptr) {
                    delete memory[var];
                }
                memory[var] = value;
            }
            ///
            RefTable::add(value);
            if(TO_REF(value)) {
                RefTable::add(TO_REF(value)->ref);
                RefTable::add_ref(TO_REF(value)->ref, TO_REF(value));
            }
            ///
        }
    }

    std::vector<std::string> init_by_string(std::string& init_string, int depth, bool allow_shadowing = false) {
        int assign_idx = 0;
        std::vector<std::string> var_names;
        std::vector<std::string> assignments = parser::parse_init(init_string, assign_idx);
        int total_assigns = assignments.size();
        var_names.reserve(total_assigns / 2);
        for(int each_assign = 0; each_assign < total_assigns; each_assign += 2) {
            set(assignments[each_assign], assignments[each_assign + 1], nullptr, VAR, false, depth, allow_shadowing);
            var_names.emplace_back(assignments[each_assign]);
        }
        return var_names;
    }

    std::string print(std::string var) {
        Value* v = get(var, {}, true, true);
        ////std::cout << v << "\n";
        //std::cout << var << " " << v << std::endl;
        //std::cout << v->print() << std::endl;
        //std::cout << "v: " << v << "\n";
        if(v == nullptr) {
            errors::undefined_var(call_stack, var, var);
        }
        return v->print();
    }
};

std::string pretty_print(std::string var, Value* value = nullptr, uint64_t indent = 0, bool single = true, uint64_t step = 2) {
    std::stringstream text;
    if(value == nullptr) {
        value = VarTable::get(var, {}, true, true);
    }

    if(TO_NULL(value)) {
        text << style::style["red"] << style::style["italic"] << "null" << style::style["reset"];
    }
    else if(TO_NUM(value)) {
        text << style::style["yellow"] << TO_NUM(value)->val << style::style["reset"];
    }
    else if(TO_NUM(value)) {
        std::string str = TO_STR(value)->str;
        if(single) {
            str = str.substr(1, str.size() - 2);
        }
        text << style::style["green"] << str << style::style["reset"];
    }
    else if(TO_LIST(value)) {
        bool multi_line = false;
        text << "[ ";
        int size = TO_LIST(value)->items.size();
        if(size > 9) {
            multi_line = true;
            indent += step;
            text << "\n" << std::string(indent, ' ');
        }
        for(int each = 0; each < size; each++) {
            if(multi_line) {
                if((each + 1) % 5 == 0) {
                    text << "\n";
                    text << std::string(indent, ' ');
                }
            }
            text << pretty_print("", TO_LIST(value)->items[each], indent, false);
            if(each == (size - 1)) {
                continue;
            }
            text << ", ";
        }
        indent -= step;
        if(multi_line) {
            text << "\n" << std::string(indent, ' ');
        }
        if(!multi_line) {
            text << " ";
        }
        text << "]";
    }
    else if(TO_DICT(value)) {
        text << "#(\n";
        indent += step;
        int total_keys = TO_DICT(value)->keys.size();
        for(int each = 0; each < total_keys; each++) {
            std::string key = TO_DICT(value)->keys[each];
            text << std::string(indent, ' ')
                    << style::style["bold"] << key << style::style["reset"]
                    << " -> "
                    << pretty_print("", TO_DICT(value)->dict[key], indent, false);
            if(each == (total_keys - 1)) {
                text << "\n";
                continue;
            }
            text << ",\n";
        }
        indent -= step;
        text << std::string(indent, ' ') << ")";
    }
    return text.str();
}

bool compare_atom(Value* first, Value* second) {
    if((TO_NUM(first) && TO_STR(second)) || (TO_STR(first) && TO_NUM(second))) {
        return false;
    }
    if(TO_NUM(first) && TO_NUM(second)) {
        if(TO_NUM(first)->val != TO_NUM(second)->val) {
            return false;
        }
    }
    else if(TO_STR(first) && TO_STR(second)) {
        if(strcmp(TO_STR(first)->str, TO_STR(second)->str)) {
            return false;
        }
    }
    return true;
}

bool compare(Value* first, Value* second) {
    bool result = true;

    if(TO_LIST(first) && TO_LIST(second)) {
        if(TO_LIST(first)->items.size() != TO_LIST(second)->items.size()) {
            return false;
        }
        int size = TO_LIST(first)->items.size();
        for(int idx = 0; idx < size; idx++) {
            Value* each_a = TO_LIST(first)->items[idx];
            Value* each_b = TO_LIST(second)->items[idx];
            if((TO_NUM(each_a) && TO_NUM(each_b)) || (TO_STR(each_a) && TO_STR(each_b))) {
                result = compare_atom(each_a, each_b);
                if(!result) {
                    return false;
                }
            }
            else if((TO_LIST(each_a) && TO_LIST(each_b)) || (TO_DICT(each_a) && TO_DICT(each_b))) {
                result = compare(each_a, each_b);
                if(!result) {
                    return false;
                }
            }
            else {
                result = false;
            }
        }
    }
    else if(TO_DICT(first) && TO_DICT(second)) {
        if(TO_DICT(first)->keys.size() != TO_DICT(second)->keys.size()) {
            return false;
        }
        std::unordered_map<std::string, Value*>::iterator itr;
        for(itr = TO_DICT(first)->dict.begin(); itr != TO_DICT(first)->dict.end(); itr++) {
            Value* each_a = itr->second;
            Value* each_b = TO_DICT(second)->dict[itr->first];
            if(each_b == nullptr) {
                return false;
            }
            if((TO_NUM(each_a) && TO_NUM(each_b)) || (TO_STR(each_a) && TO_STR(each_b))) {
                result = compare_atom(each_a, each_b);
                if(!result) {
                    return false;
                }
            }
            else if((TO_LIST(first) && TO_LIST(second)) || (TO_DICT(first) && TO_DICT(second))) {
                result = compare(each_a, each_b);
                if(!result) {
                    return false;
                }
            }
            else {
                result = false;
            }
        }
    }
    else {
        result = false;
    }

    return result;
}

bool compare(std::string first, std::string second) {
    Value* a = nullptr;
    Value* b = nullptr;
    bool result = true;
    bool a_temp = false, b_temp = false;
    if(/*first[0] == '$'*/ parser::is_var(first)) {
        a = VarTable::get(first, {}, true, true);
    }
    else if(first[0] == '[') {
        a = new List(first);
        a_temp = true;
    }
    else if(first[0] == '#' && first[1] == '(') {
        a = new Dict(first);
        a_temp = true;
    }

    if(/*second[0] == '$'*/ parser::is_var(second)) {
        b = VarTable::get(second, {}, true, true);
    }
    else if(second[0] == '[') {
        b = new List(second);
        b_temp = true;
    }
    else if(second[0] == '#' && second[1] == '(') {
        b = new Dict(second);
        b_temp = true;
    }

    result = compare(a, b);

    if(a_temp) {
        delete a;
    }
    if(b_temp) {
        delete b;
    }

    return result;
}

bool compare(Value* first, std::string second) {
    Value* b = nullptr;
    bool temp = false;
    if(/*second[0] == '$'*/ parser::is_var(second)) {
        b = VarTable::get(second, {}, true, true);
    }
    if(second[0] == '[') {
        b = new List(second);
        temp = true;
    }
    else if(second[0] == '#' && second[1] == '(') {
        b = new Dict(second);
        temp = true;
    }

    bool result = compare(first, b);

    if(temp) {
        delete b;
    }

    return result;
}