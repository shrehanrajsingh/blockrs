#if !defined(SERVER_H)
#define SERVER_H

#if defined(_WIN32)

#else
#include "nw.hpp"
#endif

namespace rs::block
{
std::string fetch_GET (const std::string &_Host, int _Port,
                       const std::string &_Path = "/", std::string _Body = "",
                       std::string _Route = "");

std::string fetch_POST (const std::string &_Host, int _Port,
                        const std::string &_Path = "/", std::string _Body = "",
                        std::string _Route = "");

std::string fetch (const std::string &_Host, int _Port,
                   const std::string &_ReqType = "GET",
                   const std::string &_Path = "/", std::string _Body = "",
                   std::string _Route = "");

std::string fetch (const std::string &_URL,
                   const std::string &_ReqType = "GET",
                   const std::string &_Path = "/", std::string _Body = "",
                   std::string _Route = "");
} // namespace rs::block

#endif // SERVER_H
