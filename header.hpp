#if !defined(HEADER_H)
#define HEADER_H

#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// C headers
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

#endif // HEADER_H
