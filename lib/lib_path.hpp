#pragma once
#include <deque>
#include <sstream>
#include <filesystem>

namespace lib {
    /*std::string get_dir(std::string given_file) {
        int path_len = given_file.size();
        while(path_len--) {
            if(given_file[path_len] == '/') {
                break;
            }
        }
        std::string dir_name = given_file.substr(0, path_len);
        return dir_name;
    }*/

    std::string replace_preceeding(std::string passed) {
        std::string rel_path = passed;
        std::string current = std::filesystem::current_path();
        int current_size = current.size();
        if(passed.substr(0, current_size - 1) == current) {
            std::cout << "works" << std::endl;
            rel_path = passed.substr(current_size);
        }
        return rel_path;
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

    std::string get_path(std::string given_path) {
        std::deque<std::string> abs;
        std::string current = std::filesystem::current_path();
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