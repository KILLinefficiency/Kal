#pragma once

namespace preproc {
std::string trim_leading(const std::string& line) {
        int size = line.size();
        int white_space_len = 0;
        for(int line_index = 0; line_index < size; line_index++) {
            if((line[line_index] == ' ') || (line[line_index] == '\t')) {
                white_space_len++;
                continue;
            }
            break;
        }

        int rest_str_len = size - white_space_len;
        return line.substr(white_space_len, rest_str_len);
    }
}
