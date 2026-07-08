#pragma once

#include <cstdlib>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <filesystem>
#include <thread>
#include <atomic>
#include <chrono>
#include <fstream>
#include <utility>
#include <unordered_map>

#include "globals.hpp"

#define TimeNow std::chrono::steady_clock::now
using TimePoint = std::chrono::steady_clock::time_point;
using TimeDuration = std::chrono::duration<double>;

const std::string GIT = "git";
const std::string PROJECT_FILE = "project.kal";
const char* KAL_PKG = std::getenv("KAL_PKG");
std::atomic<uint64_t> subpackage_count = 0;

namespace pkg {
    void install_project(std::string, bool);

    std::vector<std::pair<std::string, std::thread>> threads;
    std::unordered_map<std::string, bool> list, tracker;

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

        uint64_t threads_size = threads.size();
        for(std::string pkg_label : pkg_labels) {
            std::string pkg_url = prepare_url(pkg_label);
            std::string install_path = std::string(KAL_PKG) + "/" + get_pkg_name(pkg_label);

            if(!tracker[pkg_url]) {
                threads.push_back(std::pair<std::string, std::thread> {
                    install_path,
                    std::thread(clone, pkg_url, install_path)
                });

                tracker[pkg_url] = true;
                if(first) {
                    list[pkg_url] = true;
                }
            }
            else if(subpackage_count > 0) {
                subpackage_count--;
            }
        }

        uint64_t current_size = threads.size();
        for(uint64_t index = threads_size; index < current_size; index++) {
            if(threads[index].second.joinable()) {
                threads[index].second.join();
                install_project(threads[index].first + "/" + PROJECT_FILE, false);
            }
        }
    }

    void log_stats(uint64_t pkg_size, uint64_t subpkg_count, double duration) {
        printf(
            "\nTotal Packages:%6ld\nTotal Sub-packages: %ld\nFinished In:%12.2lfs\n",
            pkg_size, subpkg_count, duration
        );
    }

    void install_project(std::string proj_path, bool first = true) {
        if(std::filesystem::exists(proj_path)) {
            TimePoint start = TimeNow();

            std::ifstream proj_file(proj_path);
            std::stringstream proj_contents;
            proj_contents << proj_file.rdbuf();
            proj_file.close();
            std::string proj_properties = proj_contents.str();

            Dict* proj = new Dict(proj_properties, globals);
            // FIX: Handle if "packages" is not present.
            std::vector<Value*> packages = dynamic_cast<List*>(proj->dict["packages"])->items;

            uint64_t pkg_count = packages.size();
            if(!first) {
                subpackage_count += pkg_count;
            }
            std::vector<std::string> pkg_labels;
            pkg_labels.reserve(pkg_count);

            for(Value*& each_pkg : packages) {
                std::string pkg = std::string(dynamic_cast<String*>(each_pkg)->str);
                int label_size = pkg.size();
                pkg_labels.push_back(pkg.substr(1, label_size - 2));
            }

            fetch(pkg_labels, first);
            delete proj;

            TimePoint end = TimeNow();
            TimeDuration duration = end - start;
            if(first) {
                log_stats(packages.size(), subpackage_count.load(), duration.count());
            }
        }
    }

    void install(std::vector<std::string> pkg_labels, bool sync = true) {
        TimePoint start = TimeNow();

        fetch(pkg_labels);

        TimePoint end = TimeNow();
        TimeDuration duration = end - start;
        log_stats(pkg_labels.size(), subpackage_count.load(), duration.count());

        if(sync) {
            sync_project_file();
            std::cout << "\nSynced " << PROJECT_FILE << "\n";
        }
    }
}