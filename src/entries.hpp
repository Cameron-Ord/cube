#pragma once
#include "alias.hpp"

struct strvec_view
{
    strvec_view(strvec &entrylist, strvec::iterator &pos)
        : current(pos), end(entrylist.end()), start(entrylist.begin()) {}
    strvec::iterator current;
    strvec::iterator end;
    strvec::iterator start;
};

struct contents
{
    contents(strvec paths, strvec filenames, bool is_valid, bool is_empty)
        : entry_paths(paths), entry_filenames(filenames), valid(is_valid),
          empty(is_empty) {}
    strvec entry_paths;
    strvec entry_filenames;
    bool valid;
    bool empty;
};

strvec::iterator get_next_entry(strvec_view iter);
contents get_directory_contents(const std::string &path);
