#pragma once
#include <deque>
#include <sstream>
#include <filesystem>

namespace lib {
    void ensure_extension(std::string& text, std::string ext) {
        if(ext[0] != '.') {
            ext = '.' + ext;
        }
        int ext_size = ext.size();
        int text_size = text.size();
        if(text_size < ext_size || text.substr(text_size - ext_size) != ext) {
            text += ext;
        }
    }

    std::string replace_extension(std::string& text, std::string ext) {
        if(ext[0] != '.') {
            ext = '.' + ext;
        }
        int last_index = text.size() - 1;
        while(last_index >= 0 && last_index--) {
            if(text[last_index] == '.') {
                break;
            }
        }
        std::string file_name = text.substr(0, last_index);
        file_name += ext;
        return file_name;
    }

    std::string get_dir(std::string given_path) {
        std::stringstream path(given_path);
        std::stringstream dir;
        std::deque<std::string> addr;
        std::string token;
        while(std::getline(path, token, '/')) {
            addr.push_back(token);
            //std::cout << token << std::endl;
        }
        addr.pop_back();
        while(!addr.empty()) {
            dir << addr.front() << '/';
            addr.pop_front();
        }
        return dir.str();
    }

    /*std::string get_path(std::string given_path) {
        std::filesystem::path file_path = given_path;
        std::string absoulte_path = std::filesystem::absolute(file_path);
        return absoulte_path;
    }*/

    std::string get_path(std::string given_path, std::string current = "") {
        std::deque<std::string> abs;
        //std::string current = "";
        if(current == "") {
            current = std::filesystem::current_path();
        }
        //std::cout << given_path << std::endl;
        std::stringstream home(current);
        //std::cout << std::filesystem::current_path() << std::endl;
        std::string token;
        while(std::getline(home, token, '/')) {
            //std::cout << token << std::endl;
            if(token != "" || current == "/") {
                abs.push_back(token);
            }
        }
        //std::cout << "now?\n";

        std::stringstream passed(given_path);
        while(std::getline(passed, token, '/')) {
            //std::cout << token << std::endl;
            //if(token != "") {
            abs.push_back(token);
            //std::cout << "passed ---\n";
            //std::cout << token << std::endl;
            //}
            if(abs.back() == ".") {
                abs.pop_back();
            } 
            if(abs.back() == "..") {
                //std::cout << "k\n";
                abs.pop_back();
                abs.pop_back();
            }
            //std::cout << "---" << std::endl;
            //std::cout << token << std::endl;
        }

        std::stringstream path;
        while(!abs.empty()) {
            path << '/' << abs.front();
            //std::cout << '/' << abs.front() << std::endl;
            abs.pop_front();
        }
        //std::cout << path.str() << std::endl;
        return path.str();
    }

}