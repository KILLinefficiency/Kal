#pragma once

#include <queue>
#include <vector>

#include "../var.hpp"

namespace lib {
    void list_push(std::string list_name, std::string value, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        if(TO_LIST(list.value)) {
            TO_LIST(list.value)->items.emplace_back(make_value(eval(value, globals), globals));
        }
        list.gc();
    }

    Value* list_len(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        Value* size_value = make_value(std::to_string(size), globals);
        list.gc();
        return size_value;
    }

    Value* list_first(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        Value* first = copy(TO_LIST(list.value)->items[0]);
        list.gc();
        return first;
    }

    Value* list_last(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        Value* last = copy(TO_LIST(list.value)->items[size - 1]);
        list.gc();
        return last;
    }

    void list_pop(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        delete TO_LIST(list.value)->items[size - 1];
        TO_LIST(list.value)->items.erase(TO_LIST(list.value)->items.begin() + size - 1);
        list.gc();
    }

    void list_pop_first(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            std::cerr << "error\n";
        }
        delete TO_LIST(list.value)->items[0];
        TO_LIST(list.value)->items.erase(TO_LIST(list.value)->items.begin());
        list.gc();
    }

    void list_reverse(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        int half_size = size / 2;

        Value* temp = nullptr;
        for(int index = 0; index < half_size; index++) {
            temp = TO_LIST(list.value)->items[index];
            TO_LIST(list.value)->items[index] = TO_LIST(list.value)->items[size - index - 1];
            TO_LIST(list.value)->items[size - index - 1] = temp;
        }
        list.gc();
    }

    Value* list_extend(std::vector<std::string> values, Globals& globals) {
        Value* extended_list = new List();
        std::queue<BoxedValue> all_lists;
        int total_items = 0;
        for(std::string& each : values) {
            BoxedValue list = get_or_make(each, globals);
            all_lists.push(list);
            total_items += TO_LIST(list.value)->items.size();
        }
        TO_LIST(extended_list)->items.reserve(total_items);
        while(!all_lists.empty()) {
            BoxedValue top_box = all_lists.front();
            List* top = TO_LIST(top_box.value);
            for(Value*& each_val : top->items) {
                TO_LIST(extended_list)->items.emplace_back(copy(each_val));
            }
            top_box.gc();
            all_lists.pop();
        }

        return extended_list;
    }
}