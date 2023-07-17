#include <iostream>
#include <fstream>
#include <vector>

#include "config.hpp"

namespace kast {
    void encode(std::string name, std::vector<Token>& tokens) {
        std::ofstream kast(name, std::ios::binary);
        uint64_t total_tokens = tokens.size();
        kast.write(reinterpret_cast<char*>(&total_tokens), sizeof(total_tokens));
        for(Token& tok : tokens) {
            uint64_t head_size = tok.head.size(),
                init_size = tok.init.size(),
                values_size = tok.values.size(),
                target_size = tok.target.size();

            // Writing head.
            kast.write(reinterpret_cast<char*>(&head_size), sizeof(head_size));
            kast.write(tok.head.c_str(), head_size);
            /////

            // Writing init.
            kast.write(reinterpret_cast<char*>(&init_size), sizeof(init_size));
            for(std::string& init_val : tok.init) {
                uint64_t size = init_val.size();
                kast.write(reinterpret_cast<char*>(&size), sizeof(size));
                kast.write(init_val.c_str(), init_val.size());
            }
            /////

            //Writing values.
            kast.write(reinterpret_cast<char*>(&values_size), sizeof(values_size));
            for(std::string& val : tok.values) {
                uint64_t size = val.size();
                kast.write(reinterpret_cast<char*>(&size), sizeof(size));
                kast.write(val.c_str(), val.size());
            }
            /////

            //Writing target.
            kast.write(reinterpret_cast<char*>(&target_size), sizeof(target_size));
            kast.write(tok.target.c_str(), target_size);
            /////
        }
    }

    void decode(std::string name, std::vector<Token>& tokens) {
        std::ifstream kast(name, std::ios::binary);
        uint64_t total_tokens;
        kast.read(reinterpret_cast<char*>(&total_tokens), sizeof(total_tokens));

        tokens.clear();
        tokens.reserve(total_tokens);
        
        while(total_tokens--) {
            Token token;
            uint64_t head_size, init_size, val_size, target_size;

            // Reading head.
            kast.read(reinterpret_cast<char*>(&head_size), sizeof(head_size));
            //char head_buffer[head_size + 1];
            char* head_buffer = new char[head_size + 1];
            kast.read(head_buffer, head_size);
            head_buffer[head_size] = '\0';
            token.head = head_buffer;
            delete[] head_buffer;
            /////

            // Reading init.
            kast.read(reinterpret_cast<char*>(&init_size), sizeof(init_size));
            token.init.clear();
            token.init.reserve(init_size);
            while(init_size--) {
                uint64_t init_val_size;
                kast.read(reinterpret_cast<char*>(&init_val_size), sizeof(init_val_size));
                //char init_buffer[init_val_size + 1];
                char* init_buffer = new char[init_val_size + 1];
                kast.read(init_buffer, init_val_size);
                init_buffer[init_val_size] = '\0';
                token.init.emplace_back(std::string(init_buffer));
                delete[] init_buffer;
            }
            /////

            // Reading values;
            kast.read(reinterpret_cast<char*>(&val_size), sizeof(val_size));
            token.values.clear();
            token.values.reserve(val_size);
            while(val_size--) {
                uint64_t item_size;
                kast.read(reinterpret_cast<char*>(&item_size), sizeof(item_size));
                //char val_buffer[item_size + 1];
                char* val_buffer = new char[item_size + 1];
                kast.read(val_buffer, item_size);
                val_buffer[item_size] = '\0';
                token.values.emplace_back(std::string(val_buffer));
                delete[] val_buffer;
            }
            /////

            // Reading target.
            kast.read(reinterpret_cast<char*>(&target_size), sizeof(target_size));
            //char target_buffer[target_size + 1];
            char* target_buffer = new char[target_size + 1];
            kast.read(target_buffer, target_size);
            target_buffer[target_size] = '\0';
            token.target = target_buffer;
            delete[] target_buffer;
            /////

            tokens.emplace_back(token);
        }
    }
}
