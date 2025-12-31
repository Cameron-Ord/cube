#include "entries.hpp"
#include "util.hpp"
#include <filesystem>
#include <iostream>

contents get_directory_contents(const std::string &path)
{
    strvec epaths(0), enames(0);
    bool is_valid = true, is_empty = true;
    size_t i = 0;
    try {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            std::cout << entry.path().string() << std::endl;
            epaths.push_back(entry.path().string());
            enames.push_back(entry.path().filename().string());
            i++;
        }
    } catch (const std::filesystem::filesystem_error &e) {
        log_write_str("Failed to iterator directory:", e.what());
        is_valid = false;
    }
    if (i > 0) {
        is_empty = false;
    }
    return contents(epaths, enames, is_valid, is_empty);
}

strvec::iterator get_next_entry(strvec_view iter)
{
    strvec::iterator next_entry = std::next(iter.current);
    if (next_entry == iter.end) {
        next_entry = iter.start;
    }
    return next_entry;
}
