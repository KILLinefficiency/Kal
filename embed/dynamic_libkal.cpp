#include "libkal.cpp"

extern "C"
Table* make_table() {
    return new Table;
}

extern "C"
void add_to_table(Table* table, const char* key, const char* value) {
    (*table)[std::string(key)] = std::string(value);
}

extern "C"
void free_table(Table* table) {
    delete table;
}

extern "C"
Kal* make_kal() {
    return new Kal();
}

extern "C"
Value* kal_exec(Kal* kal, const char* code, Table* table) {
    return kal->exec(std::string(code), *table);
}

extern "C"
void free_kal(Kal* kal) {
    delete kal;
}