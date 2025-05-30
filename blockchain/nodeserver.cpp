#include "nodeserver.hpp"

namespace rs::block
{
NodeServer::NodeServer () : _Server_Base (), node (nullptr) {}
NodeServer::NodeServer (Node *n) : _Server_Base (), node (n) {}
NodeServer::~NodeServer ()
{
  if (fd != -1)
    this->close ();
}

void
NodeServer::setup_socket ()
{
  addrlen = sizeof (address);
  fd = socket (AF_INET, SOCK_STREAM, 0);

  if (!fd)
    {
      std::cerr << "socket creation failed";
      exit (EXIT_FAILURE);
    }

  int opt = 1;
  setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &opt, addrlen);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (get_port ());

  if (bind (fd, (sockaddr *)&address, addrlen) < 0)
    throw std::runtime_error ("bind failed");

  if (listen (fd, get_max_clients ()) < 0)
    throw std::runtime_error ("listen failed");

  std::cout << "Node Server running on http://127.0.0.1:" << get_port ()
            << std::endl;
  std::cout.flush ();
}

void
NodeServer::handle_client (int client_fd)
{
  const int buf_size = 4096;
  char buffer[buf_size]; /* Enough size to grab headers */
  int bytes_r = read (client_fd, buffer, sizeof (buffer));

  if (bytes_r > 0)
    {
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

      HttpRequest hr = parse_request (lines);

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

      for (RouteInfo &ri : routes)
        {
          std::lock_guard<std::mutex> lock (cout_mutex);
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
              break;
            }
        }
    }
}

void
NodeServer::run ()
{
  add_routes ();
  setup_socket ();
  set_is_running (true);

  while (get_is_running ())
    {
      int client_fd = accept (fd, (sockaddr *)&address, (socklen_t *)&addrlen);

      if (client_fd < 0)
        {
          if (get_is_running ())
            perror ("accept failed");
          break;
        }

      {
        std::lock_guard<std::mutex> lock (cout_mutex);
        std::cout << "Client connected! [cliend_fd: " << client_fd << ']'
                  << std::endl;
      }

      std::thread t (&NodeServer::handle_client, this, client_fd);
      threads.push_back (std::move (t));
      threads.back ().detach ();
    }

  this->close ();
}

void
NodeServer::close ()
{
  if (fd != -1)
    {
      set_is_running (false);
      ::close (fd);
      fd = -1;
      std::cout << "Server shut down..." << std::endl;
    }
}

int
NodeServer::get_port ()
{
  return this->_Server_Base::get_port ();
}

void
NodeServer::set_port (int _P)
{
  this->_Server_Base::set_port (_P);
}

int
NodeServer::get_max_clients ()
{
  return this->_Server_Base::get_max_clients ();
}

void
NodeServer::set_max_clients (int _C)
{
  this->_Server_Base::set_max_clients (_C);
}

void
NodeServer::add_route (std::string path, std::vector<std::string> ar,
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

HttpResponse
NodeServer::route_top (HttpRequest req)
{
  //   for (auto it : req.head_map)
  // {
  //   std::cout << it.second.str_repr << '\t' << it.second.value << std::endl;
  // }

  switch (req.request_type.type)
    {
    case HttpRequestTypeEnum::Get:
      {
        std::cout << "'/'::GET\nbody: " << req.body << std::endl;
      }
      break;
    case HttpRequestTypeEnum::Post:
      {
        std::cout << "'/'::POST\nbody: " << req.body << std::endl;
      }
      break;

    default:
      break;
    }

  HttpResponse resp;
  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  resp.body = ""
              "<html> <head><title> NodeServer</title></head><body>    \
              <h1> NodeServer</h1><p> Welcome to NodeServer</p></body>  \
              </html>";

  return resp;
}

HttpResponse
NodeServer::route_info (HttpRequest req)
{
  switch (req.request_type.type)
    {
    case HttpRequestTypeEnum::Get:
      {
        std::cout << "'/info'::GET\nbody: " << req.body << std::endl;
      }
      break;

    default:
      break;
    }

  HttpResponse resp;

  if (node)
    resp.status_code = HttpStatusEnum::OK;
  else
    resp.status_code = HttpStatusEnum::ServiceUnavailable;

  resp.status_message = get_status_message (resp.status_code);

  HttpHeader content_type = parse_header ("Content-Type: application/json");
  resp.head_map[content_type.name] = content_type;

  resp.add_body (node ? node->to_string ()
                      : "{\"message\": \"Node is not available. Please don't "
                        "send requests here.\"}");

  return resp;
}

void
NodeServer::add_routes ()
{
  add_route ("/", { "GET", "POST" },
             [this] (HttpRequest req) { return this->route_top (req); });

  add_route ("/info", { "GET" },
             [this] (HttpRequest req) { return this->route_info (req); });
}
} // namespace rs::block
