#include <iostream>
#include <fstream>
#include <vector>

#include "../var.hpp"

enum DataType {
    Atom,
    Lst,
    Dic
};

void write_size(std::ofstream& bin, uint64_t size) {
    bin.write(reinterpret_cast<char*>(&size), sizeof(size));
}

void read_size(std::ifstream& bin, uint64_t& size) {
    bin.read(reinterpret_cast<char*>(&size), sizeof(size));
}

void write_type(std::ofstream& bin, DataType type) {
    bin.write(reinterpret_cast<char*>(&type), sizeof(type));
}

void read_type(std::ifstream& bin, DataType& type) {
    bin.read(reinterpret_cast<char*>(&type), sizeof(type));
}

void write_atom(std::ofstream& bin, std::string atom) {
    uint64_t size = atom.size();
    write_type(bin, Atom);
    write_size(bin, size);
    bin.write(atom.c_str(), size);
}

void read_atom(std::ifstream& bin, std::string& atom) {
    uint64_t size;
    bin.read(reinterpret_cast<char*>(&size), sizeof(size));
    char* buffer = new char[size + 1];
    bin.read(buffer, size);
    buffer[size] = '\0';
    atom = buffer;
    delete[] buffer;
}

void write_list(std::ofstream& bin, Value* data) {
    List* list = dynamic_cast<List*>(data);
    write_type(bin, Lst);
    write_size(bin, list->items.size());
    for(Value*& each : list->items) {
        if(dynamic_cast<List*>(each)) {
            //continue;
            write_list(bin, each);
        }
        else if(dynamic_cast<Number*>(each)) {
            write_atom(bin, dynamic_cast<Number*>(each)->val);
        }
        else if(dynamic_cast<String*>(each)) {
            write_atom(bin, dynamic_cast<String*>(each)->str);
        }
    }
}

void read_list(std::ifstream& bin, Value*& list) {
    uint64_t size;
    read_size(bin, size);
    list = new List();
    dynamic_cast<List*>(list)->items.reserve(size);
    DataType type;
    while(size--) {
        read_type(bin, type);
        if(type == Atom) {
            std::string data;
            read_atom(bin, data);
            if(data[0] == '"') {
                dynamic_cast<List*>(list)->items.emplace_back(new String(data));
            }
            else {
                dynamic_cast<List*>(list)->items.emplace_back(new Number(data));
            }
        }
        else if(type == Lst) {
            Value* nested_list;
            read_list(bin, nested_list);
            dynamic_cast<List*>(list)->items.emplace_back(nested_list);
        }
    }
}

namespace lib {
    void serialize(std::string name, Value* value) {
        std::ofstream bin(name, std::ios::binary);
        if(dynamic_cast<Number*>(value)) {
            write_atom(bin, dynamic_cast<Number*>(value)->val);
        }
        else if(dynamic_cast<String*>(value)) {
            write_atom(bin, dynamic_cast<String*>(value)->str);
        }
        else if(dynamic_cast<List*>(value)) {
            write_list(bin, value);
        }
    }

    void serialize(std::string name, std::string var) {
        Value* value = VarTable::get(var, {}, true, true);
        serialize(name, value);
    }

    void deserialize(std::string name, std::string var) {
        std::ifstream bin(name, std::ios::binary);
        DataType type;
        //std::string data;
        read_type(bin, type);
        if(type == Atom) {
            std::string data;
            read_atom(bin, data);
            VarTable::set(var, data);
        }
        else if(type == Lst) {
            Value* list;
            read_list(bin, list);
            VarTable::set(var, "", list);
        }
        //VarTable::set(var, data);
    }
}