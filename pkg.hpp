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
#include <unordered_map>

#include "globals.hpp"
#include "lib/lib_style.hpp"

#define TimeNow std::chrono::steady_clock::now
using TimePoint = std::chrono::steady_clock::time_point;
using TimeDuration = std::chrono::duration<double>;

struct PkgInfo {
    std::string version = "";
    bool installed = false;
};

struct PkgThread {
    std::string pkg_path;
    std::thread pkg_thread;
};

struct LabelInfo {
    std::string label;
    std::string version;
};

enum PKG_STATE {
    INSTALLED,
    UPDATED
};

enum SYNC_STATE {
    CREATED,
    SYNCED
};

using PackageList = std::unordered_map<std::string, PkgInfo>;

int indent = 1, spacing = 4;

const std::string GIT = "git";
const std::string PROJECT_FILE = "project.kal";
const char* KAL_PKG = std::getenv("KAL_PKG");

std::atomic<uint64_t> package_count = 0,
    subpackage_count = 0,
    updated_count = 0,
    failed_count = 0;

namespace pkg {
    void install_project(std::string, bool);

    std::vector<PkgThread> threads;
    PackageList list, tracker;

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

    void log_kal_pkg() {
        std::cout << style::style["gray"]
            << "Installing to: " << KAL_PKG << style::style["reset"] << "\n\n";
    }

    std::string prepare_url(std::string pkg) {
        int index = 0;
        int size = pkg.size();
        int slash_count = 0;
        bool is_remote = false;

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

            if(pkg[index] == '@') {
                is_remote = true;
            }

            index++;
        }

        if(slash_count == 1 && !is_remote) {
            url = "https://www.github.com/" + pkg;
        }
        else {
            url = pkg;
        }

        return url;
    }

    std::string format_dict(Dict* project) {
        std::stringstream formatted;
        std::string last = project->keys[(project->keys).size() - 1];
        std::string sep = ",\n";
        formatted << "#(\n";

        for(std::string key : project->keys) {
            Value*& value = project->dict[key];
            formatted << std::string(indent * spacing, ' ');
            formatted << '"' << key << "\" -> ";

            if(dynamic_cast<String*>(project->dict[key])) {
                formatted << (dynamic_cast<String*>(value))->print();
            }
            else if(dynamic_cast<Dict*>(value)) {
                indent++;
                formatted << format_dict(dynamic_cast<Dict*>(value));
                indent--;
            }

            if(key == last) {
                sep = "";
            }
            formatted << sep;
        }

        formatted << "\n" << std::string((indent - 1) * spacing, ' ') << ")";
        if(indent == 1) {
            formatted << "\n";
        }

        return formatted.str();
    }

    LabelInfo parse_label(std::string label) {
        // TODO: Validate label.
        LabelInfo label_info;
        int label_size = label.size();
        int index = 0;
        while(index < label_size - 2) {
            if(label[index] == ':' && label[index + 1] == ':') {
                label_info = {
                    .label = label.substr(0, index),
                    .version = label.substr(index + 2)
                };
                return label_info;
            }
            index++;
        }

        label_info = {
            .label = label,
            .version = "latest"
        };
        return label_info;
    }

    std::string get_pkg_name(std::string pkg_label, int level = 1) {
        int end = pkg_label.size();
        int last = end - 1;

        std::string pkg_name = "";
        while(level && last) {
            while(last >= 0 && (pkg_label[last] != '/' && pkg_label[last] != ':')) {
                last--;
            }
            level--;
            if(level) {
                last--;
            }
        }

        if(last < 0) {
            // ERR:
            std::cerr << "Syntax Error\n";
            exit(1);
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

    void log_pkg(std::string pkg_name, std::string version, enum PKG_STATE state) {
        std::stringstream log;
        log << style::style["bold"];

        if(state == INSTALLED) {
            log << style::style["green"] << "[+]";
        }
        else if(state == UPDATED) {
            log << style::style["yellow"] << "[^]";
        }

        log << " " << style::style["reset"] << style::style["bold"]
            << pkg_name << style::style["blue"] << " :: "
            << style::style["reset"] << style::style["bold"]
            << version << style::style["reset"] << "\n";

        std::cout << log.str();
    }

    void log_sync(enum SYNC_STATE state) {
        std::stringstream log;
        log << "\n" << style::style["bold"] << PROJECT_FILE << " ";
        if(state == CREATED) {
            log << style::style["green"] << "created!";
        }
        else if(state == SYNCED) {
            log << style::style["yellow"] << "updated!";
        }

        log << style::style["reset"] << "\n";
        std::cout << log.str();
    }

    void clone(std::string url, std::string path, std::string version = "latest") {
        // TODO: Handle if no tags exist.
        std::stringstream latest_tag;
        latest_tag << "$("
            << GIT << " describe --tags $("
                    << GIT << " rev-list --tags --max-count=1"
                << ")"
            << ")";

        std::string latest_version = latest_tag.str();
        std::string flush_stdout = "> /dev/null 2>&1";
        std::string pkg_name = get_pkg_name(url, 2);

        if(std::filesystem::exists(path)) {
            std::stringstream cmd, checkout;

            cmd << "cd " << path << " && "
                << GIT << " switch - "
                << "> /dev/null 2>&1 ; "
                << GIT << " fetch --tags "
                << flush_stdout;

            std::string update_cmd = cmd.str();
            int update_code = std::system(update_cmd.c_str());
            std::cout << "[UPDATE CODE]: " << update_code << "\n";
 
            checkout << "cd " << path << " && "
                << GIT << " checkout "
                << ((version == "latest")
                    ? latest_version
                    : ("'" + version + "'")) << " "
                << flush_stdout;

            std::string checkout_cmd = checkout.str();
            int checkout_code = std::system(checkout_cmd.c_str());
            std::cout << "[CHECKOUT CODE]: " << checkout_code << "\n";

            log_pkg(pkg_name, version, UPDATED);
            updated_count++;

            return;
        }

        if(version != "latest") {
            std::stringstream cmd;
            cmd << GIT << " "
                << "clone "
                << "--depth=1 "
                << "--recursive "
                << "--branch "
                << "'" << version << "' "
                << url << " "
                << path << " "
                << flush_stdout;
            std::string version_cmd = cmd.str();

            int version_code = std::system(version_cmd.c_str());
            if(version_code != 0) {
                // ERR:
                std::cerr << "Version " << version << " not found!\n";
                failed_count++;
                return;
            }
        }
        else {
            std::stringstream cmd, latest;
            cmd << GIT << " "
                << "clone "
                << "--depth=1 "
                << "--recursive "
                << url << " "
                << path << " "
                << flush_stdout;
            std::string clone_cmd = cmd.str();

            int clone_code = std::system(clone_cmd.c_str());
            if(clone_code != 0) {
                // ERR:
                std::cerr << "Invalid URL\n";
                failed_count++;
                return;
            }

            latest << "cd "
                << path << " && "
                << GIT << " fetch --tags "
                << flush_stdout << " && "
                << GIT << " checkout "
                << latest_version << " "
                << flush_stdout;
            std::string latest_cmd = latest.str();            
            int latest_tag_code = std::system(latest_cmd.c_str());
            std::cout << "[LATEST TAG CODE]: " << latest_tag_code << "\n";
            // Returns 0 for both success and failure.
            if(latest_tag_code != 0) {
                // WARN:
                std::cerr << "No registered version\n";
                failed_count++;
            }
        }

        log_pkg(pkg_name, version, INSTALLED);
    }

    void sync_project_file() {
        if(std::filesystem::exists(PROJECT_FILE)) {
            std::ifstream read_proj(PROJECT_FILE);
            std::stringstream proj_contents;
            proj_contents << read_proj.rdbuf();
            read_proj.close();

            std::string proj_properties = proj_contents.str();
            Dict* proj = new Dict(proj_properties, globals);

            PackageList::iterator itr;
            for(itr = list.begin(); itr != list.end(); itr++) {
                String* version = dynamic_cast<String*>(dynamic_cast<Dict*>(proj->dict["packages"])->dict[itr->first]);

                std::string prev_version = "";
                if(version) {
                    std::string version_str = std::string(version->str);
                    int version_len = version_str.size();
                    prev_version = version_str.substr(1, version_len - 2);
                }

                if(!version) {
                    String* new_pkg_version = new String('"' + (itr->second).version + '"');
                    dynamic_cast<Dict*>(proj->dict["packages"])->keys.push_back(itr->first);
                    dynamic_cast<Dict*>(proj->dict["packages"])->dict[itr->first] = new_pkg_version;
                }
                else if(prev_version != "" && (prev_version != (itr->second).version)) {
                    String* new_pkg_version = new String('"' + (itr->second).version + '"');
                    delete version;
                    dynamic_cast<Dict*>(proj->dict["packages"])->dict[itr->first] = new_pkg_version;
                }
            }

            std::ofstream updated_proj(PROJECT_FILE);
            updated_proj << format_dict(proj);
            updated_proj.close();

            delete proj;
            log_sync(SYNCED);
        }
        else {
            Dict* proj = new Dict();
            Dict* package_list = new Dict();
            std::ofstream create_proj(PROJECT_FILE);

            PackageList::iterator itr;
            for(itr = list.begin(); itr != list.end(); itr++) {
                String* package_version = new String('"' + (itr->second).version + '"');
                package_list->keys.push_back(itr->first);
                package_list->dict[itr->first] = dynamic_cast<Value*>(package_version);
            }

            proj->keys.push_back("packages");
            proj->dict["packages"] = package_list;

            create_proj << format_dict(proj);
            create_proj.close();

            delete proj;
            log_sync(CREATED);
        }

    }

    void fetch(std::vector<LabelInfo> pkg_labels, bool first = true) {
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
        for(LabelInfo& label_info : pkg_labels) {
            std::string pkg_url = prepare_url(label_info.label);
            std::string install_path = std::string(KAL_PKG) + "/" + get_pkg_name(pkg_url);

            if(!tracker[pkg_url].installed) {
                threads.push_back(PkgThread {
                    .pkg_path = install_path,
                    .pkg_thread = std::thread(clone, pkg_url, install_path, label_info.version)
                });

                tracker[pkg_url] = PkgInfo {
                    .version = label_info.version,
                    .installed = true
                };

                if(first) {
                    list[pkg_url] = PkgInfo {
                        .version = label_info.version,
                        .installed = true
                    };
                    package_count++;
                }
                else {
                    subpackage_count++;
                }
            }
        }

        uint64_t current_size = threads.size();
        for(uint64_t index = threads_size; index < current_size; index++) {
            if(threads[index].pkg_thread.joinable()) {
                threads[index].pkg_thread.join();
                install_project(threads[index].pkg_path + "/" + PROJECT_FILE, false);
            }
        }
    }

    void fetch(std::vector<std::string> pkg_labels, bool first = true) {
        std::vector<LabelInfo> pkg_collection;
        pkg_collection.reserve(pkg_labels.size());

        for(std::string& pkg_label : pkg_labels) {
            LabelInfo pkg_info = parse_label(pkg_label);
            pkg_collection.push_back(pkg_info);
        }

        fetch(pkg_collection, first);
    }

    void log_stats(double duration) {
        std::stringstream stats;
        stats << "\n" << style::style["bold"]
            << style::style["green"] << "+ " << style::style["reset"] << style::style["bold"] << package_count.load() << " "
            << style::style["green"] << "~ " << style::style["reset"] << style::style["bold"] << subpackage_count.load() << " "
            << style::style["yellow"] << "^ " << style::style["reset"] << style::style["bold"] << updated_count.load() << " "
            << style::style["red"] << "x " << style::style["reset"] << style::style["bold"] << failed_count.load();
            // << style::style["blue"] << "[" << duration_str << "s]" << style::style["reset"];
        
        std::cout << stats.str();
        printf(" %s%s[%0.2lfs]\n", style::style["blue"].c_str(), style::style["bold"].c_str(), duration);
        std::cout << style::style["reset"];
    }

    void install_project(std::string proj_path, bool first = true) {
        if(std::filesystem::exists(proj_path)) {
            log_kal_pkg();
            TimePoint start = TimeNow();

            std::ifstream proj_file(proj_path);
            std::stringstream proj_contents;
            proj_contents << proj_file.rdbuf();
            proj_file.close();
            std::string proj_properties = proj_contents.str();

            Dict* proj = new Dict(proj_properties, globals);
            // FIX: Handle if "packages" is not present.
            Dict* packages = dynamic_cast<Dict*>(proj->dict["packages"]);

            std::vector<LabelInfo> pkg_labels;
            pkg_labels.reserve(pkg_labels.size() + package_count.load());

            for(std::string& pkg : packages->keys) {
                std::string version_str = dynamic_cast<String*>(packages->dict[pkg])->str;
                int version_len = version_str.size();
                std::string version = version_str.substr(1, version_len - 2);
                pkg_labels.push_back(LabelInfo {
                    .label = pkg,
                    .version = version
                });
            }

            fetch(pkg_labels, first);
            delete proj;

            TimePoint end = TimeNow();
            TimeDuration duration = end - start;
            if(first) {
                log_stats(duration.count());
            }
        }
        else if(first) {
            // ERR:
            std::cerr << "project.kal not found!\n";
        }
    }

    void install(std::vector<std::string> pkg_labels, bool sync = true) {
        log_kal_pkg();
        TimePoint start = TimeNow();

        fetch(pkg_labels);

        TimePoint end = TimeNow();
        TimeDuration duration = end - start;
        log_stats(duration.count());

        if(sync && (failed_count.load() == 0)) {
            sync_project_file();
        }
    }
}