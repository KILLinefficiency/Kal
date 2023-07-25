#pragma once

namespace lib {
    std::string trim_num(std::string num) {
        std::string number;
        int size = num.size();
        int index = size - 1;

        while(index && index >= 0) {
            if(num[index] == '.') {
                break;
            }
            else if(num[index] != '0') {
                index++;
                break;
            }
            index--;
        }

        if(index == 0) {
            return num;
        }

        number = num.substr(0, index);
        return number;
    }
}
