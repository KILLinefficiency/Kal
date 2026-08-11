#include <iostream>
#include <fstream>
#include <vector>

#include "globals.hpp"
#include "config.hpp"

namespace kast {
    void write_size(std::ofstream& file, uint64_t& size) {
        file.write(reinterpret_cast<char*>(&size), sizeof(size));
    }

    void write_data(std::ofstream& file, std::string& data) {
        uint64_t size = data.size();
        write_size(file, size);
        file.write(data.c_str(), size);
    }

    void write_vector(std::ofstream& file, std::vector<std::string>& data) {
        uint64_t size = data.size();
        write_size(file, size);
        for(std::string& item : data) {
            write_data(file, item);
        }
    }

    void write_jump_table(std::ofstream& file, JumpTable& jump_table) {
        uint64_t size = jump_table.size();
        write_size(file, size);
        JumpTable::iterator itr;
        for(itr = jump_table.begin(); itr != jump_table.end(); itr++) {
            uint64_t open = itr->first, end = itr->second;
            write_size(file, open);
            write_size(file, end);
        }
    }

    void write_fn_jump_table(std::ofstream& file, FnJumpTable& fn_jump_table) {
        uint64_t size = fn_jump_table.size();
        write_size(file, size);
        FnJumpTable::iterator itr;
        for(itr = fn_jump_table.begin(); itr != fn_jump_table.end(); itr++) {
            std::string fn_name = itr->first;
            JumpTable jump_table = itr->second;
            write_data(file, fn_name);
            write_jump_table(file, jump_table);
        }
    }

    template<typename Type>
    void read_size(std::ifstream& file, std::vector<Type>& vec, uint64_t& size) {
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        vec.reserve(size);
    }

    void read_size(std::ifstream& file, uint64_t& size) {
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
    }

    void read_data(std::ifstream& file, std::string& data) {
        uint64_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        char* buffer = new char[size + 1];
        file.read(buffer, size);
        buffer[size] = '\0';
        data = buffer;
        delete[] buffer;
    }

    void read_vector(std::ifstream& file, std::vector<std::string>& data) {
        uint64_t size;
        read_size(file, data, size);
        while(size--) {
            std::string item;
            read_data(file, item);
            data.emplace_back(item);
        }
    }

    void read_jump_table(std::ifstream& file, JumpTable& jump_table) {
        uint64_t size;
        read_size(file, size);
        while(size--) {
            uint64_t open, close;
            read_size(file, open);
            read_size(file, close);
            jump_table[open] = close;
        }
    }

    void read_fn_jump_table(std::ifstream& file, FnJumpTable& fn_jump_table) {
        uint64_t size;
        read_size(file, size);
        while(size--) {
            std::string fn_name;
            JumpTable jump_table;
            read_data(file, fn_name);
            read_jump_table(file, jump_table);
            fn_jump_table[fn_name] = jump_table;
        }
    }

    void encode(std::string name, std::vector<Token>& tokens, FnTable& fn, Globals& globals) {
        std::ofstream kast(name, std::ios::binary);

        write_jump_table(kast, globals.jump_table);
        write_fn_jump_table(kast, globals.fn_jump_table);

        uint64_t total_fn = fn.size();
        write_size(kast, total_fn);

        std::unordered_map<std::string, Fn*>::iterator itr;
        for(itr = fn.begin(); itr != fn.end(); itr++) {
            write_data(kast, itr->second->name);
            write_vector(kast, itr->second->init);

            uint64_t fn_body_size = itr->second->body.size();
            write_size(kast, fn_body_size);

            for(Token& token : itr->second->body) {
                write_data(kast, token.head);
                write_vector(kast, token.init);
                write_vector(kast, token.values);
                write_data(kast, token.target);
            }
        }

        uint64_t total_tokens = tokens.size();
        write_size(kast, total_tokens);

        for(Token& tok : tokens) {
            write_data(kast, tok.head);
            write_vector(kast, tok.init);
            write_vector(kast, tok.values);
            write_data(kast, tok.target);
        }
    }

    void decode(std::string name, std::vector<Token>& tokens, FnTable& fn, Globals& globals) {
        std::ifstream kast(name, std::ios::binary);

        read_jump_table(kast, globals.jump_table);
        read_fn_jump_table(kast, globals.fn_jump_table);

        uint64_t total_fn;
        read_size(kast, total_fn);

        while(total_fn--) {
            std::string fn_name;
            read_data(kast, fn_name);

            std::vector<std::string> fn_init;
            read_vector(kast, fn_init);

            uint64_t fn_body_size;
            std::vector<Token> fn_body;
            read_size(kast, fn_body, fn_body_size);

            while(fn_body_size--) {
                Token token;

                read_data(kast, token.head);
                read_vector(kast, token.init);
                read_vector(kast, token.values);
                read_data(kast, token.target);

                fn_body.emplace_back(token);
            }

            Fn* function = new Fn(fn_name, fn_init, fn_body);
            fn[fn_name] = function;
        }

        uint64_t total_tokens;
        read_size(kast, tokens, total_tokens);

        while(total_tokens--) {
            Token token;

            read_data(kast, token.head);
            read_vector(kast, token.init);
            read_vector(kast, token.values);
            read_data(kast, token.target);

            tokens.emplace_back(token);
        }
    }
}
