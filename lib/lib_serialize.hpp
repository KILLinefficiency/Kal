#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "../var.hpp"

void write_dict(std::ofstream&, Value*&);
void read_dict(std::ifstream&, Value*&);

enum DataType {
    Atom,
    Lst,
    Dic,
    Nul
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

void write_null(std::ofstream& bin) {
    write_type(bin, Nul);
}

void write_list(std::ofstream& bin, Value*& data) {
    List* list = dynamic_cast<List*>(data);
    write_type(bin, Lst);
    write_size(bin, list->items.size());
    for(Value*& each : list->items) {
        if(dynamic_cast<Dict*>(each)) {
            write_dict(bin, each);
        }
        else if(dynamic_cast<List*>(each)) {
            write_list(bin, each);
        }
        else if(dynamic_cast<Number*>(each)) {
            write_atom(bin, dynamic_cast<Number*>(each)->val);
        }
        else if(dynamic_cast<String*>(each)) {
            write_atom(bin, dynamic_cast<String*>(each)->str);
        }
        else if(dynamic_cast<Null*>(each)) {
            write_null(bin);
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
        else if(type == Dic) {
            Value* dict;
            read_dict(bin, dict);
            dynamic_cast<List*>(list)->items.emplace_back(dict);
        }
        else if(type == Nul) {
            dynamic_cast<List*>(list)->items.emplace_back(new Null());
        }
    }
}

void write_dict(std::ofstream& bin, Value*& dict) {
    write_type(bin, Dic);
    uint64_t size = dynamic_cast<Dict*>(dict)->keys.size();
    write_size(bin, size);
    for(std::string& key : dynamic_cast<Dict*>(dict)->keys) {
        write_atom(bin, key);
        Value* value = dynamic_cast<Dict*>(dict)->dict[key];
        if(dynamic_cast<List*>(value)) {
            write_list(bin, value);
        }
        else if(dynamic_cast<Dict*>(value)) {
            write_dict(bin, value);
        }
        else if(dynamic_cast<Number*>(value)) {
            write_atom(bin, dynamic_cast<Number*>(value)->val);
        }
        else if(dynamic_cast<String*>(value)) {
            write_atom(bin, dynamic_cast<String*>(value)->str);
        }
        else if(dynamic_cast<Null*>(value)) {
            write_null(bin);
        }
    }
}

void read_dict(std::ifstream& bin, Value*& dict) {
    uint64_t size;
    read_size(bin, size);
    dict = new Dict();
    dynamic_cast<Dict*>(dict)->keys.reserve(size);
    DataType type;
    while(size--) {
        read_type(bin, type);
        std::string key;
        read_atom(bin, key);
        dynamic_cast<Dict*>(dict)->keys.emplace_back(key);
        read_type(bin, type);
        if(type == Atom) {
            std::string data;
            read_atom(bin, data);
            if(data[0] == '"') {
                dynamic_cast<Dict*>(dict)->dict[key] = new String(data);
            }
            else {
                dynamic_cast<Dict*>(dict)->dict[key] = new Number(data);
            }
        }
        else if(type == Lst) {
            Value* list;
            read_list(bin, list);
            dynamic_cast<Dict*>(dict)->dict[key] = list;
        }
        else if(type == Dic) {
            Value* nested_dict;
            read_dict(bin, nested_dict);
            dynamic_cast<Dict*>(dict)->dict[key] = nested_dict;
        }
        else if(type == Nul) {
            dynamic_cast<Dict*>(dict)->dict[key] = new Null();
        }
    }
}

namespace lib {
    void serialize(std::string name, Value*& value) {
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
        else if(dynamic_cast<Dict*>(value)) {
            write_dict(bin, value);
        }
        else if(dynamic_cast<Null*>(value)) {
            write_null(bin);
        }
    }

    void serialize(std::string name, std::string var, Globals& globals) {
        Value* value = VarTable::get(var, {}, true, true, true, globals);
        serialize(name, value);
    }

    Value* deserialize(std::string name) {
        std::ifstream bin(name, std::ios::binary);
        DataType type;
        read_type(bin, type);
        if(type == Atom) {
            std::string data;
            read_atom(bin, data);
            if(data[0] == '"') {
                return new String(data);
            }
            return new Number(data);
        }
        else if(type == Lst) {
            Value* list;
            read_list(bin, list);
            return list;
        }
        else if(type == Dic) {
            Value* dict;
            read_dict(bin, dict);
            return dict;
        }
        else if(type == Nul) {
            return new Null();
        }

        return nullptr;
    }
}