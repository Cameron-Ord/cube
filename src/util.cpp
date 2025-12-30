#include "util.hpp"
#include <fstream>
#include <iostream>
void log_write_str(std::string msg, std::string err) {
  std::ofstream file("log.txt");
  if (!file) {
    std::cerr << "Failed to open file" << std::endl;
  }

  file << msg << " " << err << '\n';
  file.close();
}
