#include <filesystem>
#include <iostream>
#include <fstream>
#include "sys.hpp"

void log_write_str(std::string msg, std::string err){
  std::ofstream file("log.txt");
  if(!file){
    std::cerr << "Failed to open file" << std::endl;
  }

  file << msg << " " << err << '\n';
  file.close();
}

contents get_directory_contents(const std::string& path){
  strvec epaths(0), enames(0);
  bool is_valid = true, is_empty = true;
  size_t i = 0;
  try {
    for(const auto& entry : std::filesystem::directory_iterator(path)){
      std::cout << entry.path().string() << std::endl;
      epaths.push_back(entry.path().string());
      enames.push_back(entry.path().filename().string());
      i++;
    }
  } catch (const std::filesystem::filesystem_error& e) {
    log_write_str("Failed to iterator directory:", e.what());
    is_valid = false;
  }
  if(i > 0){
    is_empty = false;
  }
  return contents(epaths, enames, is_valid, is_empty);
}


