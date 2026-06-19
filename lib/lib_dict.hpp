#pragma once

#include "lib_string.hpp"
#include "../var.hpp"

namespace lib {
    Value* dict_len(std::string dict_name, Globals& globals) {
        BoxedValue dict = get_or_make(dict_name, globals);
        int size = TO_DICT(dict.value)->keys.size();
        Value* size_value = make_value(std::to_string(size), globals);
        dict.gc();
        return size_value;
    }

    Value* dict_keys(std::string dict_name, Globals& globals) {
        BoxedValue dict = get_or_make(dict_name, globals);
        List* keys = new List();
        keys->items.reserve(TO_DICT(dict.value)->keys.size());
        for(std::string& key : TO_DICT(dict.value)->keys) {
            keys->items.emplace_back(new String('"' + key + '"'));
        }
        dict.gc();
        return keys;
    }

    Value* dict_values(std::string dict_name, Globals& globals) {
        BoxedValue dict = get_or_make(dict_name, globals);
        List* values = new List();
        values->items.reserve(TO_DICT(dict.value)->keys.size());
        for(std::string& key : TO_DICT(dict.value)->keys) {
            values->items.emplace_back(copy(TO_DICT(dict.value)->dict[key]));
        }
        dict.gc();
        return values;
    }

    Value* dict_items(std::string dict_name, Globals& globals) {
        BoxedValue dict = get_or_make(dict_name, globals);
        List* items = new List();
        items->items.reserve(TO_DICT(dict.value)->keys.size());
        for(std::string& key : TO_DICT(dict.value)->keys) {
            List* pair = new List();
            pair->items.reserve(2);
            pair->items.emplace_back(new String('"' + key + '"'));
            pair->items.emplace_back(copy(TO_DICT(dict.value)->dict[key]));
            items->items.emplace_back(pair);
        }
        dict.gc();
        return items;
    }

    Value* dict_update(std::string dict_name, std::string other_dict, Globals& globals) {
        BoxedValue dict = get_or_make(dict_name, globals);
        BoxedValue other = get_or_make(other_dict, globals);
        Value* updated_dict = new Dict();

        for(std::string& key : TO_DICT(dict.value)->keys) {
            TO_DICT(updated_dict)->append_unique(key, true);
            TO_DICT(updated_dict)->dict[key] = copy(TO_DICT(dict.value)->dict[key]);
        }

        for(std::string& key : TO_DICT(other.value)->keys) {
            TO_DICT(updated_dict)->append_unique(key, true);
            TO_DICT(updated_dict)->dict[key] = copy(TO_DICT(other.value)->dict[key]);
        }

        dict.gc();
        other.gc();
        return updated_dict;
    }

    Value* dict_key_exists(std::string dict_name, std::string key, Globals& globals) {
        BoxedValue dict = get_or_make(dict_name, globals);
        std::string exists = "0";
        if(TO_DICT(dict.value)->dict[lib::resolve_string(key)] != nullptr) {
            exists = "1";
        }

        dict.gc();
        return new Number(exists);
    }
}