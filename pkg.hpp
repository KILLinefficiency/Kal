#pragma once

#include <cstdlib>
#include <sstream>
#include <filesystem>

const std::string GIT = "git";
const char* KAL_PKG = std::getenv("KAL_PKG");

namespace pkg {
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

    std::string get_pkg_name(std::string pkg_label) {
        int end = pkg_label.size();
        int last = end - 1;

        std::string pkg_name = "";
        if(last > 0) {
            while(last >= 0 && pkg_label[last] != '/') {
                last--;
            }

            pkg_name = pkg_label.substr(last + 1, end - last);
        }

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

    void fetch(std::string pkg_label) {
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

        std::string pkg_url = prepare_url(pkg_label);
        create_kal_pkg();

        std::stringstream cmd;
        cmd << GIT << " "
            << "clone "
            << "--depth=1 "
            << pkg_url << " "
            << KAL_PKG << "/"
            << get_pkg_name(pkg_label) << " "
            << "> /dev/null 2>&1";
        
        std::cout << "[CMD]: " << cmd.str() << "\n";
    }
}