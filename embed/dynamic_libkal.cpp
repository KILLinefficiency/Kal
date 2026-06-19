#include <cstring>

#include "libkal.cpp"

extern "C" {
    Table* new_table() {
        return new Table;
    }

    void add_to_table(Table* table, const char* key, const char* value) {
        (*table)[std::string(key)] = std::string(value);
    }

    void free_table(Table* table) {
        delete table;
    }
}

extern "C" {
    ResultList* new_list() {
        return new ResultList;
    }

    void extract_to_list(ResultList* list, Result* result) {
        for(Result& each_result : result->to_list()) {
            list->emplace_back(Result(each_result));
        }
    }

    int list_size(ResultList* list) {
        return list->size();
    }

    Result* list_index(ResultList* list, int index) {
        return new Result((*list)[index]);
    }

    void free_list(ResultList* list) {
        delete list;
    }
}

extern "C" {
    ResultDict* new_dict() {
        return new ResultDict;
    }

    void extract_to_dict(ResultDict* dict, Result* result) {
        ResultDict temp_dict = result->to_dict();
        ResultDict::iterator itr;
        for(itr = temp_dict.begin(); itr != temp_dict.end(); itr++) {
            (*dict)[itr->first] = Result(itr->second);
        }
    }

    Result* dict_key(ResultDict* dict, char const* key) {
        return new Result((*dict)[std::string(key)]);
    }

    void free_dict(ResultDict* dict) {
        delete dict;
    }
}

extern "C" {
    std::vector<std::string>* extract_keys(ResultDict* result_dict) {
        std::vector<std::string>* keys = new std::vector<std::string>;
        keys->reserve(result_dict->size());

        ResultDict::iterator itr;
        for(itr = result_dict->begin(); itr != result_dict->end(); itr++) {
            keys->emplace_back(itr->first);
        }

        return keys;
    }

    int keys_size(std::vector<std::string>* keys) {
        return keys->size();
    } 

    const char* index_keys(std::vector<std::string>* keys, int index) {
        return ((*keys)[index]).c_str();
    }

    void free_keys(std::vector<std::string>* keys) {
        delete keys;
    }
}


extern "C" {
    Result* new_result(const char* value) {
        return new Result(std::string(value));
    }

    Result* result_get_index(Result* result, int index) {
        return new Result((*result)[index]);
    }

    Result* result_get_key(Result* result, const char* key) {
        return new Result((*result)[std::string(key)]);
    }

    double result_to_number(Result* result) {
        return result->to_number();
    }

    const char* result_to_string(Result* result) {
        std::string string_object = result->to_string();
        int string_len = string_object.size();

        char* string = new char[string_len + 1];
        std::strcpy(string, string_object.c_str());

        return string;
    }

    bool result_to_null(Result* result) {
        return result->to_null();
    }

    const char* result_display(Result* result) {
        return (result->value).c_str();
    }

    void free_result(Result* result) {
        delete result;
    }
}

extern "C" {
    Kal* new_kal() {
        return new Kal();
    }

    Result* kal_exec(Kal* kal, const char* code, Table* table) {
        return new Result(kal->exec(std::string(code), *table));
    }

    void free_kal(Kal* kal) {
        delete kal;
    }
}