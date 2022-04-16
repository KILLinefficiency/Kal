namespace lib {
    bool ends_with(const std::string& major_string, const std::string& minor_string) {
        int major_string_size = major_string.size();
        int minor_string_size = minor_string.size();
        int diff_size = major_string_size - minor_string_size;

        return major_string.substr(diff_size, major_string_size - 1) == minor_string;
    }
}
