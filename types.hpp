#pragma once

#include <sstream>
#include <stack>
#include <utility>
#include "config.hpp"

class Value {
    public:
        std::string type = "";
        bool is_ref = false;
        std::stack<std::pair<Value*, int>>* shadow = nullptr;
        void init_shadow() {
            shadow = new std::stack<std::pair<Value*, int>>;
        }
        void gc_shadow() {
            delete shadow;
            shadow = nullptr;
        }
        virtual std::string print() = 0;
        virtual ~Value() {}
};

class Ref : public Value {
    public:
        std::string type = "Ref";
        Value* ref = nullptr;
        Value* parent = nullptr;
        Ref(Value*, Value* = nullptr);
        std::string print();
        ~Ref();
};

class Number : public Value {
    public:
        std::string type = "Number";
        std::string val;
        Number(std::string);
        Number();
        std::string print();
        ~Number();
};

class String : public Value {
    public:
        char* str;
        std::string type = "String";
        String();
        String(std::string);
        std::string print();
        std::string val();
        ~String();
};

class Char : public Value {
    public:
        char char_val;
        char* str;
        int index;
        Char();
        Char(String*, int);
        Char(char, char*, int);
        void change(char);
        std::string print();
        ~Char();
};

class List : public Value {
    public:
        std::vector<Value*> items;
        std::string type = "List";
        List();
        List(std::string, Globals&);
        std::string print();
        ~List();
};

class Dict : public Value {
    public:
        std::unordered_map<std::string, Value*> dict;
        std::string type = "Dict";
        std::vector<std::string> keys;
        Dict();
        Dict(std::string, Globals&);
        void append_unique(std::string, bool = false);
        std::string print();
        ~Dict();
};

class Null : public Value {
    public:
        std::string type = "Null";
        Null();
        std::string print();
        ~Null();
};

class Fn : public Value {
    public:
        std::string type = "Fn";
        Fn(std::vector<std::string>&);
        Fn(std::string&, std::vector<std::string>&, std::vector<Token>&);
        std::string print();
        std::string name;
        std::vector<std::string> init;
        std::vector<Token> body;
        ~Fn();
};

Fn::Fn(std::vector<std::string>& fn_init) {
    name = fn_init[0];
    init = std::vector<std::string>(fn_init.begin() + 1, fn_init.end() - 1);
}

Fn::Fn(std::string& fn_name, std::vector<std::string>& fn_init, std::vector<Token>& fn_body) :
    name(fn_name), init(fn_init), body(fn_body) {}

std::string Fn::print() {
    std::stringstream disp;
    disp << "(" << name << " -> ";
    int init_size = init.size();
    for(int each = 0; each < init_size; each += 2) {
        disp << init[each];

        if(init[each + 1] != "null") {
            disp << ": " << init[each + 1];
        }

        if(each != init_size - 1 && each != init_size - 2) {
            disp << ", ";
        }
    }
    disp << ")";
    return disp.str();
}

Fn::~Fn() {}