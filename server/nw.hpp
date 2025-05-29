#if !defined(NW_H)
#define NW_H

#include "../utils/str.hpp"
#include "classes.hpp"
#include "httpheader.hpp"

namespace rs::block
{
using namespace rs::util;

/**
 * IMPORTANT: For Development Use Only
 *
 * This code serves as a cross-platform development reference.
 * Server practice modifications are initially implemented here
 * before being migrated to production server implementations.
 * Not intended for use in production projects.
 *
 * To make your own server implementation,
 * copy this class definition, rename it, and implement
 * defined functions.
 */
class Server : public _Server_Base
{
  struct sockaddr_in address;
  int addrlen = 0;

public:
  Server ();
  ~Server ();

  void setup_socket () override;
  void handle_client (int) override;
  void run () override;
  void close () override;

  int get_port () override;
  void set_port (int) override;
  int get_max_clients () override;
  void set_max_clients (int) override;
};

/**
 * @brief HTTP server implementation that handles HTTP requests and responses
 *
 * HttpServer class inherits from _Server_Base and provides specific
 * functionality for handling HTTP protocol communications. It manages socket
 * connections, client requests, and server operations.
 *
 * The barebone of this class is derived from Server class
 *
 * @details The server uses IPv4 addressing (sockaddr_in) for network
 * communications. It implements all required virtual methods from the
 * _Server_Base class to provide HTTP-specific functionality.
 *
 * @note This implementation requires proper error handling when used in
 * production environments
 */
class HttpServer : public _Server_Base
{
private:
  struct sockaddr_in address;
  int addrlen = 0;

public:
  HttpServer ();
  ~HttpServer ();

  void setup_socket () override;
  void handle_client (int) override;
  void run () override;
  void close () override;

  int get_port () override;
  void set_port (int) override;
  int get_max_clients () override;
  void set_max_clients (int) override;
};
} // namespace rs::block

#endif // NW_H
