#include "server.hpp"

namespace rs::block
{
std::string
fetch_GET (const std::string &host, int port, const std::string &path,
           std::string body)
{
  int sockfd;

  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    throw std::runtime_error ("socket creation failed");

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
      sockfd = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd == -1)
        continue;

      if (connect (sockfd, p->ai_addr, p->ai_addrlen) == 0)
        break;

      close (sockfd);
    }

  if (p == nullptr)
    {
      freeaddrinfo (res);
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
  send (sockfd, req.c_str (), req.size (), 0);

  char buf[4096];
  std::string resp;
  int bytes_read;

  while ((bytes_read = read (sockfd, buf, sizeof (buf))) > 0)
    resp.append (buf, bytes_read);
  //   bytes_read = read (sockfd, buf, sizeof (buf));
  //   resp.append (buf, bytes_read);

  close (sockfd);
  freeaddrinfo (res);

  return resp;
}

std::string
fetch_POST (const std::string &host, int port, const std::string &path,
            std::string body)
{
  int sockfd;

  if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    throw std::runtime_error ("socket creation failed");

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
      sockfd = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd == -1)
        continue;

      if (connect (sockfd, p->ai_addr, p->ai_addrlen) == 0)
        break;

      close (sockfd);
    }

  if (p == nullptr)
    {
      freeaddrinfo (res);
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
  send (sockfd, req.c_str (), req.size (), 0);

  char buf[4096];
  std::string resp;
  int bytes_read;

  while ((bytes_read = read (sockfd, buf, sizeof (buf))) > 0)
    resp.append (buf, bytes_read);
  //   bytes_read = read (sockfd, buf, sizeof (buf));
  //   resp.append (buf, bytes_read);

  close (sockfd);
  freeaddrinfo (res);

  return resp;
}
} // namespace rs::block
