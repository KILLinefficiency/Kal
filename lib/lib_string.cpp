#include <vector>
#include <fstream>
#include <sstream>

namespace lib {
    bool ends_with(const std::string& major_string, const std::string& minor_string) {
        int major_string_size = major_string.size();
        int minor_string_size = minor_string.size();

        if(major_string_size < minor_string_size) {
            return false;
        }

        int diff_size = major_string_size - minor_string_size;
        return major_string.substr(diff_size, major_string_size - 1) == minor_string;
    }

    std::string read_file(std::string file_path) {
        std::ifstream source_file(file_path);
        std::stringstream file_contents;
        file_contents << source_file.rdbuf();
        source_file.close();
        return file_contents.str();
    }


    std::vector<std::string> split(std::string text, char delimiter = ' ', char escape_char = '"') {
        int len = -1;
        int begin = 0;
        int size = text.size();
        bool enable_split = true;
        std::vector<std::string> words;

        if(text[size - 1] != delimiter) {
            text += delimiter;
            size++;
        }


        for(int current_index = 0; current_index < size; current_index++) {
            if(text[current_index] == escape_char) {
                enable_split = !enable_split;
                if(enable_split) {
                    begin++;
                }
                else {
                    len -=2;
                }
            }

            len++;

            if((text[current_index] == delimiter) && enable_split) {
                words.emplace_back(text.substr(begin, len));
                begin = current_index + 1;
                len = -1;
            }
        }

        return words;
    }

}
