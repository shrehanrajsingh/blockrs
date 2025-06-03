#include "server.hpp"

namespace rs::block
{
class ScopedSocket
{
  int sockfd;

public:
  ScopedSocket () : sockfd (-1) {}
  explicit ScopedSocket (int fd) : sockfd (fd) {}

  ScopedSocket (const ScopedSocket &) = delete;
  ScopedSocket &operator= (const ScopedSocket &) = delete;

  ScopedSocket (ScopedSocket &&other) noexcept : sockfd (other.sockfd)
  {
    other.sockfd = -1;
  }

  ScopedSocket &
  operator= (ScopedSocket &&other) noexcept
  {
    if (this != &other)
      {
        close ();
        sockfd = other.sockfd;
        other.sockfd = -1;
      }
    return *this;
  }

  int
  get () const
  {
    return sockfd;
  }

  void
  reset (int newfd = -1)
  {
    if (sockfd != -1)
      ::close (sockfd);
    sockfd = newfd;
  }

  void
  close ()
  {
    if (sockfd != -1)
      {
        ::close (sockfd);
        sockfd = -1;
      }
  }

  ~ScopedSocket ()
  {
    if (sockfd != -1)
      ::close (sockfd);
  }
};

std::string
fetch_GET (const std::string &host, int port, const std::string &path,
           std::string body)
{
  ScopedSocket sockfd;

  struct addrinfo hints{}, *res, *p;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  dbg ("Host: " << host << "\nPort: " << port << "\nPath: " << path);
  size_t skip_http = 0;

  if (host.find ("http") == 0)
    skip_http = (host[4] == 's' ? 8 : 7);

  const char *skh = host.c_str () + skip_http;
  std::string port_str = std::to_string (port);

  int status = getaddrinfo (skh, port_str.c_str (), &hints, &res);

  if (status != 0)
    {
      dbg ("status: " << status);
      throw std::runtime_error (gai_strerror (status));
    }

  for (p = res; p != nullptr; p = p->ai_next)
    {
      ScopedSocket temp_sockfd (
          socket (p->ai_family, p->ai_socktype, p->ai_protocol));
      if (temp_sockfd.get () == -1)
        continue;

      if (connect (temp_sockfd.get (), p->ai_addr, p->ai_addrlen) == 0)
        {
          sockfd = std::move (temp_sockfd);
          break;
        }
    }

  freeaddrinfo (res);

  if (sockfd.get () == -1)
    {
      throw std::runtime_error ("failed to connect");
    }

  //   memset (&addr, 0, sizeof (addr));
  //   addr.sin_family = AF_INET;
  //   addr.sin_port = htons (port);
  //   memcpy ((void *)&addr.sin_addr.s_addr, server->h_addr,
  //   server->h_length);

  std::ostringstream oss;
  oss << "GET " << path << " HTTP/1.1\r\n"
      << "Host: " << host << "\r\n"
      << "Connection: close\r\n"
      << "Content-Length: " << body.size () << "\r\n\r\n"
      << body;

  std::string req = oss.str ();
  send (sockfd.get (), req.c_str (), req.size (), 0);

  char buf[4096];
  std::string resp;
  int bytes_read;

  while ((bytes_read = read (sockfd.get (), buf, sizeof (buf))) > 0)
    resp.append (buf, bytes_read);
  //   bytes_read = read (sockfd, buf, sizeof (buf));
  //   resp.append (buf, bytes_read);

  return resp;
}

std::string
fetch_POST (const std::string &host, int port, const std::string &path,
            std::string body)
{
  ScopedSocket sockfd;

  struct addrinfo hints{}, *res, *p;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  dbg ("Host: " << host << "\nPort: " << port << "\nPath: " << path);
  size_t skip_http = 0;

  if (host.find ("http") == 0)
    skip_http = (host[4] == 's' ? 8 : 7);

  const char *skh = host.c_str () + skip_http;
  std::string port_str = std::to_string (port);

  int status = getaddrinfo (skh, port_str.c_str (), &hints, &res);

  if (status != 0)
    {
      dbg ("status: " << status);
      throw std::runtime_error (gai_strerror (status));
    }

  for (p = res; p != nullptr; p = p->ai_next)
    {
      ScopedSocket temp_sockfd (
          socket (p->ai_family, p->ai_socktype, p->ai_protocol));
      if (temp_sockfd.get () == -1)
        continue;

      if (connect (temp_sockfd.get (), p->ai_addr, p->ai_addrlen) == 0)
        {
          sockfd = std::move (temp_sockfd);
          break;
        }
    }

  freeaddrinfo (res);

  if (sockfd.get () == -1)
    {
      throw std::runtime_error ("failed to connect");
    }

  //   memset (&addr, 0, sizeof (addr));
  //   addr.sin_family = AF_INET;
  //   addr.sin_port = htons (port);
  //   memcpy ((void *)&addr.sin_addr.s_addr, server->h_addr,
  //   server->h_length);

  std::ostringstream oss;
  oss << "POST " << path << " HTTP/1.1\r\n"
      << "Host: " << host << "\r\n"
      << "Connection: close\r\n"
      << "Content-Length: " << body.size () << "\r\n\r\n"
      << body;

  std::string req = oss.str ();
  send (sockfd.get (), req.c_str (), req.size (), 0);

  char buf[4096];
  std::string resp;
  int bytes_read;

  while ((bytes_read = read (sockfd.get (), buf, sizeof (buf))) > 0)
    resp.append (buf, bytes_read);
  //   bytes_read = read (sockfd, buf, sizeof (buf));
  //   resp.append (buf, bytes_read);

  return resp;
}

std::string
fetch (const std::string &host, int port, const std::string &req,
       const std::string &path, std::string body)
{
  if (req == "GET")
    {
      return fetch_GET (host, port, path, body);
    }
  else if (req == "POST")
    {
      return fetch_POST (host, port, path, body);
    }
  /* else if... */

  return "";
}

std::string
fetch (const std::string &url, const std::string &req, const std::string &path,
       std::string body)
{
  size_t colon_pos = url.rfind (':');

  if (colon_pos == std::string::npos)
    throw std::invalid_argument ("Invalid url");

  std::string host = url.substr (0, colon_pos);
  std::string port_str = url.substr (colon_pos + 1);

  int port = std::stoi (port_str);

  return fetch (host, port, req, path, body);
}
} // namespace rs::block
