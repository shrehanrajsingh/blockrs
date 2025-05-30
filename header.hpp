#if !defined(HEADER_H)
#define HEADER_H

#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// C headers
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// custom
#include "utils/str.hpp"
#include "utils/vec.hpp"

#if !defined(dbg)
#define dbg(X)                                                                \
  std::cout << __FILE__ << '(' << __LINE__ << ')' << X << std::endl;
#endif // dbg

#if !defined(KRS_API)
#define KRS_API
#endif // KRS_API

namespace rs::util
{
static void
trim_string (std::string &s)
{
  while (s.front () == ' ' || s.front () == '\t' || s.front () == '\r'
         || s.front () == '\n')
    s.erase (0, 1);

  while (s.back () == ' ' || s.back () == '\t' || s.back () == '\r'
         || s.back () == '\n')
    s.pop_back ();
}
}

#endif // HEADER_H
