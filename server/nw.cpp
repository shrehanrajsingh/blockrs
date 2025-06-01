#include "nw.hpp"
#include "../header.hpp"

namespace rs::block
{
using namespace rs::util;

Server::Server () : _Server_Base () {}
Server::~Server ()
{
  if (fd != -1)
    this->close ();
}

void
Server::setup_socket ()
{
  addrlen = sizeof (address);
  fd = socket (AF_INET, SOCK_STREAM, 0);

  if (!fd)
    {
      std::cerr << "socket failed" << std::endl;
      exit (EXIT_FAILURE);
    }

  int opt = 1;
  setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (get_port ());

  if (bind (fd, (struct sockaddr *)&address, addrlen) < 0)
    throw std::runtime_error ("bind failed");

  if (listen (fd, get_max_clients ()) < 0)
    throw std::runtime_error ("listen failed");

  std::cout << "Server running on port " << get_port () << "..." << std::endl;
}

void
Server::handle_client (int client_fd)
{
  char buffer[1024];

  while (true)
    {
      memset (buffer, 0, sizeof (buffer));
      int bytes_r = read (client_fd, buffer, sizeof (buffer));

      if (bytes_r <= 0)
        {
          // std::lock_guard<std::mutex> lock (cout_mutex);
          std::cout << "Client disconnected. [client_fd: " << client_fd << ']'
                    << std::endl;

          /* client has exited */
          std::vector<int>::iterator pos
              = std::find (client_fds.begin (), client_fds.end (), client_fd);

          if (pos != client_fds.end ())
            client_fds.erase (pos);

          break;
        }

      {
        // std::lock_guard<std::mutex> lock (cout_mutex);
        std::cout << "Received: " << buffer;
      }
    }
}

void
Server::run ()
{
  setup_socket ();
  set_is_running (true);
  while (get_is_running ())
    {
      int client_fd
          = accept (fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

      if (client_fd < 0)
        {
          if (get_is_running ())
            perror ("accept failed");
          break;
        }

      add_client (client_fd);

      {
        // std::lock_guard<std::mutex> lock (cout_mutex);
        std::cout << "New client connected! [client_fd: " << client_fd << ']'
                  << std::endl;
      }

      std::thread t (&Server::handle_client, this, client_fd);
      threads.push_back (std::move (t));
      threads.back ().detach ();
    }

  this->close ();
}

int
Server::get_port ()
{
  return this->_Server_Base::get_port ();
}

void
Server::set_port (int _P)
{
  return this->_Server_Base::set_port (_P);
}

int
Server::get_max_clients ()
{
  return this->_Server_Base::get_max_clients ();
}
void
Server::set_max_clients (int _C)
{
  this->_Server_Base::set_max_clients (_C);
}

void
Server::close ()
{
  set_is_running (false);
  ::close (fd);
  fd = -1;
  std::cout << "Server shut down...";
}

HttpServer::HttpServer () : _Server_Base () {}

HttpServer::~HttpServer ()
{
  if (fd != -1)
    this->close ();
}

int
HttpServer::get_port ()
{
  return this->_Server_Base::get_port ();
}

void
HttpServer::set_port (int _P)
{
  this->_Server_Base::set_port (_P);
}

int
HttpServer::get_max_clients ()
{
  return this->_Server_Base::get_max_clients ();
}
void
HttpServer::set_max_clients (int _C)
{
  this->_Server_Base::set_max_clients (_C);
}

void
HttpServer::setup_socket ()
{
  addrlen = sizeof (address);
  fd = socket (AF_INET, SOCK_STREAM, 0);

  if (!fd)
    {
      std::cerr << "socket failed" << std::endl;
      exit (EXIT_FAILURE);
    }

  int opt = 1;
  setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (get_port ());

  if (bind (fd, (struct sockaddr *)&address, addrlen) < 0)
    throw std::runtime_error ("bind failed");

  if (listen (fd, get_max_clients ()) < 0)
    throw std::runtime_error ("listen failed");

  std::cout << "Server running on port: " << get_port () << std::endl;
  std::cout.flush ();
}

void
HttpServer::handle_client (int client_fd)
{
  const int buf_size = 4096;
  char buffer[buf_size]; /* Enough size to grab headers */
  int bytes_r = read (client_fd, buffer, sizeof (buffer));

  if (bytes_r > 0)
    {
      //   if (bytes_r == buf_size)
      //     {
      //       /* possibility that more data exists */
      //       //   TODO
      //     }
      //   else
      //     {
      std::vector<std::string> lines;
      lines.push_back ("");

      char *bp = (char *)buffer;

      for (int i = 0; i < bytes_r; i++)
        {
          if (*bp == '\r' && *(bp + 1) == '\n')
            {
              bp += 2;
              if (*bp == '\r' && *(bp + 1) == '\n')
                break;
              else
                lines.push_back ("");

              continue;
            }

          lines.back () += *bp++;
        }

      //   for (size_t i = 0; i < lines.get_size (); i++)
      //     {
      //       std::cout << i << ": " << lines[i] << std::endl;
      //     }

      HttpRequest hr = parse_request (lines);
      // std::cout << hr.request_type.str_repr << '\t' << hr.request_type.url
      //           << '\n';

      try
        {
          HttpHeader content_length
              = hr.get_header (HttpHeaderEnum::ContentLength);

          std::string vs = content_length.value;
          while (vs.front () == '\n' || vs.front () == '\t'
                 || vs.front () == '\r' || vs.front () == ' ')
            vs.erase (0, 1);

          while (vs.back () == '\n' || vs.back () == '\t' || vs.back () == '\r'
                 || vs.back () == ' ')
            vs.pop_back ();

          int cl = atoi (vs.c_str ());
          int dif = bp - (char *)buffer;

          if (dif + cl > buf_size)
            {
              /* we need to read more to fetch entire body */
              char *rem_buf = new char[cl];
              int bytes_alr_read = bytes_r - dif;
              int rem_bytes = cl - bytes_alr_read;

              if (bytes_alr_read > 0)
                {
                  memcpy (rem_buf, bp, bytes_alr_read);
                }

              int total_read = bytes_alr_read;
              while (total_read < cl)
                {
                  int bytes_read = read (client_fd, rem_buf + total_read,
                                         cl - total_read);
                  if (bytes_read <= 0)
                    {
                      goto rem_client;
                      break;
                    }
                  total_read += bytes_read;
                }

              hr.body
                  = std::string (std::string (rem_buf, total_read).c_str ());
              delete[] rem_buf;
            }
          else
            {
              /* entire body is in buffer */
              hr.body = std::string (bp);
            }
        }
      catch (const std::exception &e)
        {
          /**
           * Content-Length header does not exist
           * Thus we will assume body does not exist
           */
        }

      // {
      //   std::lock_guard<std::mutex> lock (cout_mutex);
      //   if (hr.validate ())
      //     {
      //       for (auto &&i : hr.headers)
      //         {
      //           std::stringstream ss;
      //           ss << "name (" << i.str_repr.size () << "): " << i.str_repr
      //              << ", value: " << i.value;

      //           std::cout << ss.str () << std::endl;
      //         }

      //       std::cout << "Body: " << hr.body << std::endl;
      //     }
      //   else
      //     {
      //       std::cerr << "Invalid HTTP request. Ignoring..." << std::endl;
      //     }
      // }

      bool handled_client = false;
      for (RouteInfo &ri : routes)
        {
          // std::lock_guard<std::mutex> lock (cout_mutex);
          auto iv
              = std::find (ri.allowed_requests.begin (),
                           ri.allowed_requests.end (), hr.request_type.type);
          if (iv != ri.allowed_requests.end ()
              && hr.request_type.url == ri.path)
            {
              HttpResponse resp = ri.callback (hr);
              std::string rs = resp.to_string ();

              send (client_fd, rs.c_str (), rs.size (), 0);
              ::close (client_fd);
              handled_client = true;
              break;
            }
        }

      if (!handled_client)
        {
          HttpResponse resp404;
          resp404.status_code = HttpStatusEnum::NotFound;
          resp404.status_message = get_status_message (resp404.status_code);

          std::string rs = resp404.to_string ();

          send (client_fd, rs.c_str (), rs.size (), 0);
          ::close (client_fd);
        }
      // }
    }
  else
    {
    rem_client:;
      /* client has exited */
      std::vector<int>::iterator pos
          = std::find (client_fds.begin (), client_fds.end (), client_fd);

      if (pos != client_fds.end ())
        client_fds.erase (pos);
    }
}

void
HttpServer::run ()
{
  add_routes ();
  setup_socket ();
  set_is_running (true);

  while (get_is_running ())
    {
      int client_fd
          = accept (fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

      if (client_fd < 0)
        {
          if (get_is_running ())
            perror ("accept failed");
          break;
        }

      add_client (client_fd);

      {
        // std::lock_guard<std::mutex> lock (cout_mutex);
        std::cout << "Client connected! [client_fd: " << client_fd << ']'
                  << std::endl;
      }

      std::thread t (&HttpServer::handle_client, this, client_fd);
      threads.push_back (std::move (t));
      threads.back ().detach ();
    }

  this->close ();
}

void
HttpServer::close ()
{
  set_is_running (false);
  ::close (fd);
  fd = -1;
  std::cout << "Server shut down...";
}

void
HttpServer::add_route (std::string path, std::vector<std::string> ar,
                       std::function<HttpResponse (HttpRequest)> cb)
{
  RouteInfo ri;
  ri.path = path;
  ri.callback = cb;

  std::vector<HttpRequestTypeEnum> hars;
  for (std::string &i : ar)
    hars.push_back (parse_request_type_enum (i));

  ri.allowed_requests = hars;
  routes.push_back (ri);
}

} // namespace rs::block
