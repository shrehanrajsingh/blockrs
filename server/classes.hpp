#if !defined(SERVER_CLASSES_H)
#define SERVER_CLASSES_H

#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

// #include "../utils/vec.hpp"

namespace rs::block
{
// using namespace rs::util;

class _Server_Base
{
protected:
  std::atomic<bool> is_running;
  std::vector<int> client_fds;
  std::vector<std::thread> threads;

  std::mutex cout_mutex;
  int fd;
  int port;
  int max_clients;

public:
  _Server_Base () : is_running (false), fd (-1), port (8080), max_clients (10)
  {
  }

  virtual void
  handle_client (int)
  {
  }

  virtual void setup_socket () {};

  inline std::vector<int> &
  get_client_fds ()
  {
    return client_fds;
  }

  void
  add_client (int _c)
  {
    client_fds.push_back (_c);
  }

  int
  get_client (size_t i)
  {
    return client_fds[i];
  }

  virtual void
  run ()
  {
    is_running = true;
  }

  virtual void
  close ()
  {
    is_running = false;
  }

  bool
  get_is_running ()
  {
    return is_running;
  }

  void
  set_is_running (bool _V)
  {
    is_running = _V;
  }

  virtual int
  get_port ()
  {
    return port;
  }

  virtual void
  set_port (int _P)
  {
    port = _P;
  }

  virtual int
  get_max_clients ()
  {
    return max_clients;
  }

  virtual void
  set_max_clients (int _C)
  {
    max_clients = _C;
  }

  ~_Server_Base () {}
};
} // namespace rs::block

#endif // SERVER_CLASSES_H
