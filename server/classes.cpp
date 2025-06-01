#include "classes.hpp"
#include <fstream>
#include <iostream>

namespace rs::block
{
std::string
read_file (std::string path)
{
  std::ifstream file (path);
  if (!file)
    throw std::runtime_error ("Cannot open file: " + path);

  std::stringstream buffer;
  buffer << file.rdbuf ();
  return buffer.str ();
}
} // namespace rs::block
