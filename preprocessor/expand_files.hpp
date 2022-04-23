#include <vector>

#include "trim_spaces.hpp"
#include "../lib/lib_string.hpp"

namespace preproc {
    std::string expand_files(std::string file_path) {
        std::string file_contents = lib::read_file(file_path);
        std::vector<std::string> file_lines = lib::split(file_contents, '\n');
        int file_lines_count = file_lines.size(); 

        for(int current_line = 0; current_line < file_lines_count; current_line++) {
            std::cout << file_lines[current_line] << std::endl;
            file_lines[current_line] = preproc::trim_leading(file_lines[current_line]);
            if(file_lines[current_line] == "") {
                continue;
            }
            std::cout << file_lines[current_line] << std::endl;;
        }
        return "";
    }
}
