#pragma once

#include <cstdlib>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>
#include <fstream>
#include <unordered_map>

#include "globals.hpp"

#define TimeNow std::chrono::steady_clock::now
using TimePoint = std::chrono::steady_clock::time_point;
using TimeDuration = std::chrono::duration<double>;

const std::string GIT = "git";
const std::string PROJECT_FILE = "project.kal";
const char* KAL_PKG = std::getenv("KAL_PKG");


namespace pkg {
    std::vector<std::thread> threads;
    std::unordered_map<std::string, bool> list;

    bool check() {
        std::string git_cmd = GIT + " > /dev/null 2>&1";
        return std::system(git_cmd.c_str()) == 256;
    }

    bool valid_char(char c) {
        return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            (c == '/' || c == ':' || c == '.' || c == '@');
    }

    std::string prepare_url(std::string pkg) {
        int index = 0;
        int size = pkg.size();
        int slash_count = 0;

        std::string url;

        while(index < size) {
            if(index == 0 && (size > 8) && pkg.substr(index, 8) == "https://") {
                while(index < size) {
                    if(!valid_char(pkg[index])) {
                        // ERR:
                        std::cerr << "Invalid Package Name!\n";
                        exit(1);
                    }
                    index++;
                }
                url = pkg;
                break;
            }

            if(!valid_char(pkg[index])) {
                // ERR:
                std::cerr << "Invalid Package Name!\n";
                exit(1);
            }

            if(pkg[index] == '/') {
                slash_count++;
            }

            index++;
        }

        if(slash_count == 1) {
            url = "https://www.github.com/" + pkg;
        }

        return url;
    }

    std::string get_pkg_name(std::string pkg_label, int level = 1) {
        int end = pkg_label.size();
        int last = end - 1;

        std::string pkg_name = "";
        while(level && last) {
            while(last >= 0 && pkg_label[last] != '/') {
                last--;
            }
            level--;
            if(level) {
                last--;
            }
        }

        pkg_name = pkg_label.substr(last + 1, end - last);
        return pkg_name;
    }

    void create_kal_pkg() {
        // TODO: Handle ~.
        if(std::filesystem::exists(KAL_PKG)) {
            if(!std::filesystem::is_directory(KAL_PKG)) {
                // ERR:
                std::cout << "Conflicting KAL_PKG!\n";
                exit(1);
            }
            return;
        }
        std::filesystem::create_directories(KAL_PKG);
    }

    void clone(std::string url, std::string path) {
        std::stringstream cmd;
        cmd << GIT << " "
            << "clone "
            << "--depth=1 "
            << url << " "
            << path << " "
            << "> /dev/null 2>&1";
        std::string shell_cmd = cmd.str();
        std::system(shell_cmd.c_str());
        std::cout << ("[+] " + get_pkg_name(url, 2) + "\n");
    }

    bool exists_in_list(std::vector<Value*> list, std::string value) {
        std::string val = '"' + value + '"';
        for(Value*& each : list) {
            if(dynamic_cast<String*>(each) && (dynamic_cast<String*>(each)->print() == val)) {
                return true;
            }
        }

        return false;
    }

    void sync_project_file() {
        if(std::filesystem::exists(PROJECT_FILE)) {
            std::ifstream read_proj(PROJECT_FILE);
            std::stringstream proj_contents;
            proj_contents << read_proj.rdbuf();
            read_proj.close();

            std::string proj_properties = proj_contents.str();
            Dict* proj = new Dict(proj_properties, globals);

            std::unordered_map<std::string, bool>::iterator itr;
            for(itr = list.begin(); itr != list.end(); itr++) {
                std::vector<Value*> existing_packages = dynamic_cast<List*>(proj->dict["packages"])->items;
                if(!exists_in_list(existing_packages, itr->first)) {
                    String* new_pkg = new String('"' + itr->first + '"');
                    dynamic_cast<List*>(proj->dict["packages"])->items.push_back(new_pkg);
                }
            }

            std::ofstream updated_proj(PROJECT_FILE);
            updated_proj << proj->print() << "\n";
            updated_proj.close();

            delete proj;
        }
        else {
            Dict* proj = new Dict();
            List* package_list = new List();
            std::ofstream create_proj(PROJECT_FILE);

            std::unordered_map<std::string, bool>::iterator itr;
            for(itr = list.begin(); itr != list.end(); itr++) {
                String* package_name = new String('"' + itr->first + '"');
                package_list->items.emplace_back(dynamic_cast<Value*>(package_name));
            }

            proj->keys.push_back("packages");
            proj->dict["packages"] = package_list;

            create_proj << proj->print() << "\n";
            create_proj.close();

            delete proj;
        }

    }

    void fetch(std::vector<std::string> pkg_labels, bool first = true) {
        if(first) {
            if(!KAL_PKG) {
                // ERR:
                std::cerr << "KAL_PKG not set!\n";
                exit(1);
            }

            if(!check()) {
                // ERR:
                std::cerr << "Git needed!\n";
                exit(1);
            }

            create_kal_pkg();
        }

        TimePoint start = TimeNow();

        for(std::string pkg_label : pkg_labels) {
            std::string pkg_url = prepare_url(pkg_label);
            std::string install_path = std::string(KAL_PKG) + "/" + get_pkg_name(pkg_label);
            threads.push_back(std::thread(clone, pkg_url, install_path));
            list[pkg_url] = true;
        }

        for(std::thread& thread : threads) {
            thread.join();
        }

        TimePoint end = TimeNow();
        TimeDuration duration = end - start;
        printf("\nTotal Packages: %ld\nFinished In:    %0.2lfs\n", pkg_labels.size(), duration.count());

        if(first) {
            sync_project_file();
            std::cout << "\nSynced " << PROJECT_FILE << "\n";
        }
    }
}