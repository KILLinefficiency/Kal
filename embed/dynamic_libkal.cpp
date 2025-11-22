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
    Kal* new_kal() {
        return new Kal();
    }

    Value* kal_exec(Kal* kal, const char* code, Table* table) {
        return kal->exec(std::string(code), *table);
    }

    void free_kal(Kal* kal) {
        delete kal;
    }
}