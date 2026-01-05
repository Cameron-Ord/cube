#pragma once
#include "audio/adef.hpp"

#include <string>
#include <vector>

struct path {
  std::wstring wstring;
  std::u8string u8string;
};

using pathvec = std::vector<path>;

struct paths_view
{
    paths_view(pathvec &entrylist, pathvec::iterator &pos) : current(pos), end(entrylist.end()), start(entrylist.begin())
    {
    }
    pathvec::iterator current;
    pathvec::iterator end;
    pathvec::iterator start;
};

struct contents
{
    contents(pathvec paths, pathvec filenames, bool is_valid, bool is_empty)
        : entry_paths(paths), entry_filenames(filenames), valid(is_valid), empty(is_empty)
    {
    }
    pathvec entry_paths;
    pathvec entry_filenames;
    bool valid;
    bool empty;
};

pathvec::iterator get_next_entry(paths_view iter);
contents get_directory_contents(const std::string &path);
audio_data read_file(file_data file);
file_data open_file(const path &path);
