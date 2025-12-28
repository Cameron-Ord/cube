#pragma once
#include "alias.hpp"

struct contents {
  contents(strvec paths, strvec filenames, bool is_valid, bool is_empty) 
    : entry_paths(paths), entry_filenames(filenames), valid(is_valid), empty(is_empty) {}
  strvec entry_paths;
  strvec entry_filenames;
  bool valid;
  bool empty;
};

contents get_directory_contents(const std::string& path);
