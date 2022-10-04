#pragma once

namespace lib {
    std::string display_num(double num) {
        if(num == int(num)) {
            return std::to_string(int(num));
        }
        return std::to_string(num);
    }
}