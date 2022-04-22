#pragma once

std::string remove_comments(const std::string& line) {
    if(line[0] == ';') {
        return "";
    }
    
    bool enable_removal = true;
    int required_len = 0;
    int line_size = line.size();
    for(int char_index = 0; char_index < line_size; char_index++) {
        if(line[char_index] == '"') {
            enable_removal = !enable_removal;
        }
        if((line[char_index] == ';') && enable_removal) {
            break;
        }
        required_len++;
    }

    return line.substr(0, required_len);
}
