#pragma once

#include <queue>
#include <vector>
#include <cstring>

#include "../var.hpp"

namespace lib {
    void list_push(std::string list_name, std::string value, bool allow_insert, int insert_index, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        Value* new_value = make_value(eval(value, globals), globals);
        if(TO_LIST(list.value) && !allow_insert) {
            TO_LIST(list.value)->items.emplace_back(new_value);
        }
        else if(TO_LIST(list.value) && allow_insert) {
            TO_LIST(list.value)->items.insert(TO_LIST(list.value)->items.begin() + insert_index, new_value);
        }
        list.gc();
    }

    Value* list_len(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = 0;
        if(TO_LIST(list.value)) {
            size = TO_LIST(list.value)->items.size();
        }
        else if(TO_DICT(list.value)) {
            size = TO_DICT(list.value)->keys.size();
        }
        else if(TO_STR(list.value)) {
            size = std::strlen(TO_STR(list.value)->str) - 2;
        }
        Value* size_value = make_value(std::to_string(size), globals);
        list.gc();
        return size_value;
    }

    Value* list_first(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            errors::empty_list(globals, list_name);
        }
        Value* first = copy(TO_LIST(list.value)->items[0]);
        list.gc();
        return first;
    }

    Value* list_last(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            errors::empty_list(globals, list_name);
        }
        Value* last = copy(TO_LIST(list.value)->items[size - 1]);
        list.gc();
        return last;
    }

    void list_pop(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            errors::empty_list(globals, list_name);
        }
        delete TO_LIST(list.value)->items[size - 1];
        TO_LIST(list.value)->items.erase(TO_LIST(list.value)->items.begin() + size - 1);
        list.gc();
    }

    void list_pop_first(std::string list_name, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);
        int size = TO_LIST(list.value)->items.size();
        if(size == 0) {
            errors::empty_list(globals, list_name);
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

    Value* list_flat_by_ptr(Value* list, int level, Globals& globals) {
        Value* flat_list = new List();

        for(uint64_t index = 0; index < TO_LIST(list)->items.size(); index++) {
            List* nested_list = TO_LIST(TO_LIST(list)->items[index]);
            if(nested_list) {
                int nested_size = nested_list->items.size();
                for(int nested_index = 0; nested_index < nested_size; nested_index++) {
                    TO_LIST(flat_list)->items.emplace_back(copy(TO_LIST(nested_list)->items[nested_index]));
                }
            }
            else {
                TO_LIST(flat_list)->items.emplace_back(copy(TO_LIST(list)->items[index]));
            }
        }

        if(level > 1) {
            Value* flat_list_top = flat_list;
            flat_list = list_flat_by_ptr(flat_list, level - 1, globals);
            delete flat_list_top;
        }

        return flat_list;
    }

    Value* list_flat(std::string list_name, int level, Globals& globals) {
        BoxedValue list = get_or_make(list_name, globals);

        Value* flat_list = list_flat_by_ptr(list.value, level, globals);
        list.gc();

        return flat_list;
    }
}