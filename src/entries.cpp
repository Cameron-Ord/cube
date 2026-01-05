#include "entries.hpp"
#include "util.hpp"
#include <filesystem>
#include <iostream>

contents get_directory_contents(const std::string &path)
{
    pathvec epaths(0), enames(0);
    bool is_valid = true, is_empty = true;
    size_t i = 0;
    try {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            epaths.push_back({entry.path().wstring(), entry.path().u8string()});
            enames.push_back({entry.path().filename().wstring(), entry.path().filename().u8string()});
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

pathvec::iterator get_next_entry(paths_view iter)
{
    pathvec::iterator next_entry = std::next(iter.current);
    if (next_entry == iter.end) {
        next_entry = iter.start;
    }
    return next_entry;
}
