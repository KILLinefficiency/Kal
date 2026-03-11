#pragma once

#include "../var.hpp"

namespace lib {
    void list_push(std::string list_name, std::string value, Globals& globals) {
        FetchedValue list = get_or_make(list_name, globals);
        if(TO_LIST(list.value)) {
            TO_LIST(list.value)->items.emplace_back(make_value(eval(value, globals), globals));
        }
        list.gc();
    }

    Value* list_len(std::string list_name, Globals& globals) {
        FetchedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        Value* size_value = make_value(std::to_string(size), globals);
        list.gc();
        return size_value;
    }

    Value* list_first(std::string list_name, Globals& globals) {
        FetchedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        Value* first = copy(TO_LIST(list.value)->items[0]);
        list.gc();
        return first;
    }

    Value* list_last(std::string list_name, Globals& globals) {
        FetchedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        Value* last = copy(TO_LIST(list.value)->items[size - 1]);
        list.gc();
        return last;
    }

    void list_pop(std::string list_name, Globals& globals) {
        FetchedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        delete TO_LIST(list.value)->items[size - 1];
        TO_LIST(list.value)->items.erase(TO_LIST(list.value)->items.begin() + size - 1);
        list.gc();
    }

    void list_pop_first(std::string list_name, Globals& globals) {
        FetchedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        delete TO_LIST(list.value)->items[0];
        TO_LIST(list.value)->items.erase(TO_LIST(list.value)->items.begin());
        list.gc();
    }
}