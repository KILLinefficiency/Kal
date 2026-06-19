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
#define TO_CHAR(value) (dynamic_cast<Char*>(value))
#define TO_LIST(value) (dynamic_cast<List*>(value))
#define TO_DICT(value) (dynamic_cast<Dict*>(value))
#define TO_NULL(value) (dynamic_cast<Null*>(value))
#define TO_REF(value)  (dynamic_cast<Ref*>(value))

Value* copy(Value*);

Value* make_value(std::string value, Globals& globals) {
    Value* val = nullptr;
    if((value[0] >= '0' && value[0] <= '9') || value[0] == '-') {
        val = new Number(value);
    }
    else if(value[0] == '"') {
        val = new String(value);
    }
    else if(value[0] == '[') {
        val = new List(value, globals);
    }
    else if(value[0] == '#' && value[1] == '(') {
        val = new Dict(value, globals);
    }
    else if(value == "null") {
        val = new Null();
    }
    else if(parser::is_var(value)) {
        val = VarTable::get(value, {}, false, false, true, globals);
    }
    return val;
}

Ref::Ref(Value* val, Value* p) : ref(val), parent(p) {}
std::string Ref::print() {
    return ref->print();
}
Ref::~Ref() {}

Null::Null() {}
std::string Null::print() {
    return "null";
}
Null::~Null() {}

Number::Number() {}

Number::Number(std::string Val) : val(Val) {}
std::string Number::print() {
    return val;
}
Number::~Number() {}

String::String() {}

String::String(std::string Str) {
    int len = Str.size();
    str = new char[len + 1];
    strcpy(str, Str.c_str());
    str[len] = '\0';
}

std::string String::print() {
    return str;
}

std::string String::val() {
    return lib::resolve_string(std::string(str));
}

String::~String() {
    delete[] str;
    str = nullptr;
}

Char::Char() {}

Char::Char(char current_char, char* current_string, int current_index) : char_val(current_char), str(current_string), index(current_index + 1) {}

Char::Char(String* string, int idx) : char_val(string->str[idx + 1]), str(string->str), index(idx + 1) {}

std::string Char::print() {
    return '"' + std::string(1, char_val) + '"';
}

void Char::change(char new_char) {
    char_val = new_char;
    str[index] = new_char;
}

Char::~Char() {}

List::List() {}

List::List(std::string list, Globals& globals) {
    int index = 0;
    std::vector<std::string> values = parser::parse_list(list, index);
    for(std::string v : values) {
        if(v[0] == '.' && v[1] == '.' && v[2] == '.') {
            v = v.substr(3);
        }
        items.emplace_back(make_value(eval(v, globals), globals));
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

Dict::Dict() {}

Dict::Dict(std::string dict_val, Globals& globals) {
    int index = 0;
    std::vector<std::string> kv = parser::parse_map(dict_val, index);
    int size = kv.size();
    keys.reserve(size / 2);
    for(int i = 0; i < size; i += 2) {
        if(kv[i].size() > 3 && kv[i][0] == '.' && kv[i][1] == '.' && kv[i][2] == '.') {
            std::string spread_dict_name = kv[i].substr(3);
            Dict* spread_dict = TO_DICT(VarTable::get(spread_dict_name, {}, true, true, true, globals));
            for(std::string& key : spread_dict->keys) {
                append_unique(key, true);
                dict[key] = copy(spread_dict->dict[key]);
            }
            continue;
        }
        if(kv[i][0] == '"' && kv[i][kv[i].size() - 1] == '"') {
            kv[i] = lib::resolve_string(kv[i]);
        }
        append_unique(kv[i], true);
        dict[kv[i]] = make_value(eval(kv[i + 1], globals), globals);
    }
}

void Dict::append_unique(std::string key, bool del_val) {
    if(std::find(keys.begin(), keys.end(), key) == keys.end()) {
        keys.emplace_back(key);
    }
    else if(del_val) {
        if(dict[key] != nullptr) {
            delete dict[key];
            dict[key] = nullptr;
        }
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

Value* copy(Value* value) {
    Value* duplicate = nullptr;

    if(TO_NUM(value)) {
        duplicate = new Number(TO_NUM(value)->val);
    }
    else if(TO_STR(value)) {
        duplicate = new String(TO_STR(value)->str);
    }
    else if(TO_CHAR(value)) {
        duplicate = new Char(TO_CHAR(value)->char_val, TO_CHAR(value)->str, TO_CHAR(value)->index);
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
        duplicate = new Ref(TO_REF(value)->ref);
    }

    if(duplicate == nullptr) { std::cout << "cannot determine type\n"; }

    return duplicate;
}

namespace RefTable {
    std::unordered_map<Value*, uint64_t> variables;
    std::vector<std::vector<Ref*>> references;

    void add(Value*& val) {
        bool exists = variables[val] != 0;
        if(!exists) {
            variables[val] = references.size() + 1;
            references.emplace_back(std::vector<Ref*> {});
        }
    }

    void add_ref(Value*& var, Ref* var_ref) {
        bool exists = variables[var] != 0;
        if(exists) {
            int idx = variables[var] - 1;
            references[idx].emplace_back(var_ref);
        }
    }

    void change_var(Value* old_var, Value* new_var) {
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

namespace VarTable {
    void gc_value(std::string name, Value*& val, Globals& globals) {
        int& depth = globals.depth;
        Memory& memory = globals.memory;

        if(val != nullptr) {
            if(val->shadow != nullptr && val->shadow->size() != 0) {
                std::pair<Value*, int> top = val->shadow->top();
                if(top.second >= depth) {
                    delete top.first;
                    val->shadow->pop();
                    if(val->shadow->size() == 0) {
                        val->gc_shadow();
                    }
                }
            }
            else if(globals.scope[name] >= depth) {
                delete val;
                memory[name] = nullptr;
            }
        }
    }

    void gc(Globals& globals) {
        int& depth = globals.depth;
        Memory& memory = globals.memory;

        Memory::iterator itr, end = memory.end();
        for(itr = memory.begin(); itr != end; itr++) {
            gc_value(itr->first, itr->second, globals);
        }
        if(depth == 0) {
            Functions::gc();
        }
    }

    void gc_by_names(std::vector<std::string>& var_names, Globals& globals) {
        Memory& memory = globals.memory;
        for(std::string& name : var_names) {
            gc_value(name, memory[name], globals);
        }
    }

    Value* get(std::string name, std::vector<std::string> symbols, bool update, bool for_print, bool get_original, Globals& globals) {
        int& depth = globals.depth;
        Memory& memory = globals.memory;

        if(name[0] >= '0' && name[0] <= '9') {
            return nullptr;
        }
        if(name != "" && globals.inert_table[name] != "" && !globals.inert_hit[name]) {
            std::string Name = name;
            globals.inert_hit[Name] = true;
            set(Name, globals.inert_table[Name], nullptr, VAR, false, depth, false, globals);
        }

        if(name != "" && memory[name] != nullptr) {
            Value* fetched_var = memory[name];
            Value* ret_var = nullptr;
            if(fetched_var != nullptr && fetched_var->shadow != nullptr && fetched_var->shadow->size() != 0) {
                ret_var = fetched_var->shadow->top().first;
                return (for_print) ? ret_var : copy(ret_var);
            } 
        }

        if(symbols.size() == 0) {
            symbols = get_var_with_indices(name);
        }

        std::string var_name = symbols[0];
        bool is_ref = false;
        if(var_name[0] == '&') {
            is_ref = true;
            var_name = var_name.substr(1);
        }
        Value* var = memory[var_name];
        int size = symbols.size();
        int bound = size;

        if(var == nullptr) {
            return var;
        }
        Value* p = nullptr;
        if(size > 1) {
            if(update && !for_print) {
                bound--;
            }
            for(int i = 1; i < bound; i++) {
                if(TO_REF(var)) {
                    var = TO_REF(var)->ref;
                }
                std::string access = symbols[i];
                if(parser::is_var(access)) {
                    Value* access_ptr = VarTable::get(symbols[i], {}, true, true, true, globals);
                    access = access_ptr->print();
                }
                if((access[0] >= '0' && access[0] <= '9') || (access[0] == '-')) {
                    int index = std::stoi(access);
                    if(TO_LIST(var)) {
                        int size = TO_LIST(var)->items.size();
                        if(index < 0) {
                            index = size + index;
                        }
                        if((index >= size) || (index < 0)) {
                            errors::index_error(globals, symbols[i]);
                        }
                        var = TO_LIST(var)->items[index];
                    }
                    else if(TO_STR(var)) {
                        var = new Char(TO_STR(var), index);
                        return var;
                    }
                }
                else if(access[0] == '"' && access[access.size() - 1] == '"') {
                    std::string key = lib::resolve_string(access);
                    if(TO_DICT(var) && update && (i == (bound - 1)) && (TO_DICT(var)->dict[key] == nullptr)) {
                        return var;
                    }
                    var = TO_DICT(var)->dict[key];
                    if(var == nullptr) {
                        errors::key_error(globals, key);
                    }
                }
                if(!update && bound == 2) {
                    p = memory[var_name];
                }
                else if(!update && i == size - 2) {
                    p = var;
                }
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

    void unpack(std::string list, std::string structure, Value* data_ptr, Globals& globals) {
        int& depth = globals.depth;

        int index = 0;
        Value* packed_items = nullptr;
        bool is_literal = false;
        if(list[0] == '#' && list[1] == '(') {
            int end = list.size();
            list[1] = '[';
            list[end - 1] = ']';
            index++;
        }
        std::vector<std::string> items = parser::parse_list(list, index);
        uint64_t len = items.size();
        if(structure != "" && data_ptr == nullptr) {
            structure = eval(structure, globals);
            if(parser::is_var(structure)) {
                packed_items = get(structure, {}, true, true, true, globals);
            }
            else {
                if(structure[0] == '[') {
                    packed_items = new List(structure, globals);
                }
                else if(structure[0] == '#' && structure[1] == '(') {
                    packed_items = new Dict(structure, globals);
                }
                is_literal = true;
            }
        }
        else {
            packed_items = data_ptr;
        }

        if(TO_LIST(packed_items)) {
            if(len > (TO_LIST(packed_items))->items.size()) {
                // ERR:
                errors::items_unpack(globals);
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
                    unpack(items[i], "", TO_LIST(packed_items)->items[i], globals);
                }
                else {
                    set(items[i], "", TO_LIST(packed_items)->items[i], VAR, false, depth, false, globals);
                }
            }
        }
        else if(TO_DICT(packed_items)) {
            if(len > TO_DICT(packed_items)->keys.size()) {
                // ERR:
                errors::items_unpack(globals);
                if(is_literal) {
                    delete packed_items;
                }
                return;
            }
            for(uint64_t j = 0; j < len; j++) {
                if(TO_DICT(packed_items)->dict[items[j]] == nullptr) {
                    errors::key_error(globals, items[j]);
                    if(is_literal) {
                        delete packed_items;
                    }
                    return;
                }
                set(items[j], "", TO_DICT(packed_items)->dict[items[j]], VAR, false, depth, false, globals);
            }
        }

        if(is_literal) {
            delete packed_items;
        }
    }

    void set(std::string var, std::string data, Value* data_ptr, Type type, bool disallow_copy, int depth, bool allow_shadowing, Globals& globals) {
        Memory& memory = globals.memory;

        bool is_shadowed = false;
        if(var[0] == '[' || (var[0] == '#' && var[1] == '(')) {
            unpack(var, data, data_ptr, globals);
            return;
        }
        if(type == INERT) {
            globals.inert_table[var] = data;
            globals.inert_hit[var] = false;
            return;
        }
        if(data != "") {
            data = eval(data, globals);
        }

        if(memory[var] == nullptr) {
            globals.scope[var] = depth;
        }

        Value* ptr = VarTable::get(var, {}, true, true, true, globals);
        if(ptr != nullptr && !allow_shadowing && parser::is_var(var)) {
            if(data != "" && parser::is_var(data)) {
                Value* d_ptr = VarTable::get(data, {}, true, true, true, globals);
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
                TO_NUM(ptr)->val = data;
                return;
            }
            else if(TO_STR(ptr) && data != "") {
                strcpy(TO_STR(ptr)->str, data.c_str());
                return;
            }
            else if(TO_CHAR(ptr)) {
                TO_CHAR(ptr)->change(data[1]);
            }
        }
        if(var[0] == '&') {
            errors::invalid_ref_assign(globals);
        }

        Value* value = nullptr;
        if(data == "" && data_ptr != nullptr) {
            value = disallow_copy ? data_ptr : copy(data_ptr);
        }
        else if(data == "null") {
            value = new Null();
        }
        else if(parser::is_var(data)) {
            value = get(data, {}, false, false, true, globals);
            if(TO_REF(value)) {
                RefTable::add(TO_REF(value)->ref);
                RefTable::add_ref((TO_REF(value)->ref), TO_REF(value));
            }
        }
        else {
            value = make_value(data, globals);
        }

        if(allow_shadowing && memory[var] != nullptr) {
            if(memory[var]->shadow == nullptr) {
                memory[var]->init_shadow();
            }
            // if(ScopeTable::scope[var] == depth) {
            //     // ERR:
            //     errors::var_redeclare(globals, var);
            // }
            memory[var]->shadow->push({ value, depth });
            is_shadowed = true;
        }

        if(ptr != nullptr && !allow_shadowing && parser::is_var(var)) {
            std::string v_name = var;
            if(memory[v_name] != nullptr) {
                if(TO_REF(memory[v_name])) {
                    Memory::iterator itr;
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
                        else if(TO_DICT(parent_val)) {
                            Dict* p_dict = TO_DICT(parent_val);
                            std::unordered_map<std::string, Value*>::iterator itr;
                            for(itr = p_dict->dict.begin(); itr != p_dict->dict.end(); itr++) {
                                if(prev_val == itr->second) {
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
                std::vector<std::string> syms = get_var_with_indices(expand_var(var, globals));
                std::string last_symbol = lib::resolve_string(syms[syms.size() - 1]);
                if(TO_DICT(ptr) && (TO_DICT(ptr)->dict[last_symbol] == nullptr)) {
                    TO_DICT(ptr)->append_unique(last_symbol, true);
                    TO_DICT(ptr)->dict[last_symbol] = value;
                    return;
                }
                Value* v = get("", syms, true, false, true, globals);


                Value* ov = get("", syms, false, false, false, globals);
                bool is_ref = false;
                if(TO_REF(ov)) {
                    is_ref = true;
                }
                delete ov;

                if(TO_LIST(v)) {
                    if(is_ref) {
                        Value* tmp = TO_REF((TO_LIST(v)->items[std::stoi(last_symbol)]))->ref;
                        bool found = false;
                        Memory::iterator itr;
                        for(itr = memory.begin(); itr != memory.end(); itr++) {
                            if(itr->second == tmp) {
                                memory[itr->first] = value;
                                found = true;
                                break;
                            }
                        }
                        if(!found) {
                            Value* tmp_parent = TO_REF((TO_LIST(v)->items[std::stoi(last_symbol)]))->parent;
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
                        TO_REF((TO_LIST(v)->items[std::stoi(last_symbol)]))->ref = value;
                        RefTable::change_var(tmp, value);
                        delete tmp;
                    }
                    else {
                        Value* prev_val = TO_LIST(v)->items[std::stoi(last_symbol)];
                        TO_LIST(v)->items[std::stoi(last_symbol)] = value;
                        RefTable::change_var(prev_val, value);
                        delete prev_val;
                    }
                }
                else if(TO_DICT(v)) {
                    if(is_ref) {
                        Value* tmp = TO_REF(TO_DICT(v)->dict[last_symbol])->ref;
                        bool found = false;
                        Memory::iterator itr;
                        for(itr = memory.begin(); itr != memory.end(); itr++) {
                            if(itr->second == tmp) {
                                found = true;
                                break;
                            }
                        }
                        if(!found) {
                            Value* tmp_parent = TO_REF(TO_DICT(v)->dict[last_symbol])->parent;
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
                        last_symbol = lib::resolve_string(last_symbol);
                        Value* old = TO_DICT(v)->dict[last_symbol];
                        RefTable::change_var(old, value);
                        delete old;
                        TO_DICT(v)->dict[last_symbol] = value;
                        TO_DICT(v)->append_unique(last_symbol);
                    }
                }
            }
            RefTable::add(value);
            if(TO_REF(value)) {
                RefTable::add(TO_REF(value)->ref);
                RefTable::add_ref(TO_REF(value)->ref, TO_REF(value));
            }
        }
        else {
            if(!is_shadowed) {
                if(memory[var] != nullptr) {
                    delete memory[var];
                }
                memory[var] = value;
            }
            RefTable::add(value);
            if(TO_REF(value)) {
                RefTable::add(TO_REF(value)->ref);
                RefTable::add_ref(TO_REF(value)->ref, TO_REF(value));
            }
        }
    }

    std::vector<std::string> init_by_string(std::string& init_string, bool allow_shadowing, Globals& globals) {
        int& depth = globals.depth;

        int assign_idx = 0;
        std::vector<std::string> var_names;
        std::vector<std::string> assignments = parser::parse_init(init_string, assign_idx);
        int total_assigns = assignments.size();
        var_names.reserve(total_assigns / 2);
        for(int each_assign = 0; each_assign < total_assigns; each_assign += 2) {
            set(assignments[each_assign], assignments[each_assign + 1], nullptr, VAR, false, depth, allow_shadowing, globals);
            var_names.emplace_back(assignments[each_assign]);
        }
        return var_names;
    }

    std::string print(std::string var, Globals& globals) {
        Value* v = get(var, {}, true, true, true, globals);
        if(v == nullptr) {
            errors::undefined_var(globals, var);
        }
        return v->print();
    }
};

List* make_range(double begin, double end, double step) {
    List* list = new List();
    int size = (end - begin + 1) / step;
    if(size < 0) {
        size = -size;
    }
    list->items.reserve(size);
    while(begin < end) {
        list->items.push_back(new Number(std::to_string(begin)));
        begin += step;
    }

    return list;
}

std::string pretty_print(Globals& globals, std::string var, Value* value = nullptr, uint64_t indent = 0, bool single = true, uint64_t step = 2) {
    std::stringstream text;
    if(value == nullptr) {
        value = VarTable::get(var, {}, true, true, true, globals);
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
            text << pretty_print(globals, "", TO_LIST(value)->items[each], indent, false);
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
                    << pretty_print(globals, "", TO_DICT(value)->dict[key], indent, false);
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

bool compare(std::string first, std::string second, Globals& globals) {
    Value* a = nullptr;
    Value* b = nullptr;
    bool result = true;
    bool a_temp = false, b_temp = false;
    if(parser::is_var(first)) {
        a = VarTable::get(first, {}, true, true, true, globals);
    }
    else if(first[0] == '[') {
        a = new List(first, globals);
        a_temp = true;
    }
    else if(first[0] == '#' && first[1] == '(') {
        a = new Dict(first, globals);
        a_temp = true;
    }

    if(parser::is_var(second)) {
        b = VarTable::get(second, {}, true, true, true, globals);
    }
    else if(second[0] == '[') {
        b = new List(second, globals);
        b_temp = true;
    }
    else if(second[0] == '#' && second[1] == '(') {
        b = new Dict(second, globals);
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

bool compare(Value* first, std::string second, Globals& globals) {
    Value* b = nullptr;
    bool temp = false;
    if(parser::is_var(second)) {
        b = VarTable::get(second, {}, true, true, true, globals);
    }
    if(second[0] == '[') {
        b = new List(second, globals);
        temp = true;
    }
    else if(second[0] == '#' && second[1] == '(') {
        b = new Dict(second, globals);
        temp = true;
    }

    bool result = compare(first, b);

    if(temp) {
        delete b;
    }

    return result;
}

struct BoxedValue {
    Value* value = nullptr;
    bool to_gc = false;

    void gc() {
        if(to_gc && value != nullptr) {
            delete value;
            value = nullptr;
        }
    }
};

BoxedValue get_or_make(std::string var, Globals& globals) {
    BoxedValue fetched_value;

    if(parser::is_var(var)) {
        fetched_value.value = VarTable::get(var, {}, true, true, true, globals);
    }
    else {
        fetched_value.value = make_value(var, globals);
        fetched_value.to_gc = true;
    }

    return fetched_value;
}

std::string get_type(std::string var, Globals& globals) {
    BoxedValue value = get_or_make(var, globals);
    std::string type = "";

    if(TO_NUM(value.value)) {
        type = TO_NUM(value.value)->type;
    }
    else if(TO_STR(value.value)) {
        type = TO_STR(value.value)->type;
    }
    else if(TO_LIST(value.value)) {
        type = TO_LIST(value.value)->type;
    }
    else if(TO_DICT(value.value)) {
        type = TO_DICT(value.value)->type;
    }
    else if(TO_NULL(value.value)) {
        type = TO_NULL(value.value)->type;
    }

    value.gc();
    return type;
}