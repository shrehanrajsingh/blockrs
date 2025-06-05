#if !defined(HEADER_H)
#define HEADER_H

#ifdef _WIN32
#error "Win32 platforms are not supported."
#endif

#include <fstream>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <openssl/sha.h>

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

/* #include <gc.h> */

#if !defined(dbg)
#define dbg(X)                                                                \
  std::cout << __FILE__ << '(' << __LINE__ << ')' << X << std::endl;
#endif // dbg

#if !defined(KRS_API)
#define KRS_API
#endif // KRS_API

#if !defined(BKRS_SERVER_URL)
#define BKRS_SERVER_URL ("http://127.0.0.1")
#endif

namespace rs::util
{
static void
trim_string (std::string &s)
{
  auto start = s.find_first_not_of (" \t\n\r\f\v");
  if (start == std::string::npos)
    {
      s.clear ();
      return;
    }

  auto end = s.find_last_not_of (" \t\n\r\f\v");
  s = s.substr (start, end - start + 1);
}
}

#endif // HEADER_H
