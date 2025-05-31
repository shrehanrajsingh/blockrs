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

  dbg ("Node Server running on " << BKRS_SERVER_URL << ':' << get_port ());
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

      bool handled_client = false;
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
          return;
        }

      add_client (client_fd);

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

  // clang-format off
  resp.add_body (
    HTML_RAW (
<html>
<head>
<title>BlockRS::NodeServer</title>
<style>
:root {
--bg-color: #f8f8f8;
--text-color: #333;
--heading-color: #2c3e50;
--link-color: #3498db;
}

@media (prefers-color-scheme: dark) {
:root {
--bg-color: #222;
--text-color: #f0f0f0;
--heading-color: #60a5fa;
--link-color: #60a5fa;
}
}

body.light-mode {
--bg-color: #f8f8f8;
--text-color: #333;
--heading-color: #2c3e50;
--link-color: #3498db;
}

body.dark-mode {
--bg-color: #222;
--text-color: #f0f0f0;
--heading-color: #60a5fa;
--link-color: #60a5fa;
}

body {
font-family: Arial, sans-serif;
line-height: 1.6;
margin: 0;
padding: 20px;
background-color: var(--bg-color);
color: var(--text-color);
transition: background-color 0.3s, color 0.3s;
}
.container {
max-width: 800px;
margin: 0 auto;
padding: 20px;
}
h1 {
color: var(--heading-color);
}
a {
color: var(--link-color);
}

.theme-toggle {
background: none;
border: 1px solid var(--text-color);
border-radius: 4px;
color: var(--text-color);
padding: 5px 10px;
cursor: pointer;
margin-bottom: 10px;
}
</style>
</head>
<body>
<div class="container">
<button class="theme-toggle" id="theme-toggle">Toggle Dark Mode</button>
<h1>NodeServer</h1>
<p>
Welcome to NodeServer. This server is designed to run as a blockchain
node.
<br />
You can use this server to connect to and participate in blockchain
networks.
<br />
<br />
<strong>Note:</strong> This is a template for a blockchain node server.
You can customize it to suit your specific blockchain requirements.
<br />
<strong>Important:</strong> This node server is not intended for
production use. It is meant for testing and development purposes only.
<br />
<strong>Warning:</strong> Do not use this node for any sensitive or
confidential transactions. Always verify security before production use.
<br />
<strong>Disclaimer:</strong> The developer of this node server is not
responsible for any loss or damage caused by the use of this server. Use
it at your own risk.
<br />
<strong>License:</strong> This server is licensed under the MIT License.
You can use it for any purpose, but you must include the original
license and copyright notice in any derivative works.
<br />
<strong>Contact:</strong> If you have any questions or feedback, please
contact the developer at
<a href="mailto:shrehanofficial@gmail.com">his email</a>
</p>
</div>

<script>
window.onload = () => {
const toggle = document.getElementById("theme-toggle");

const savedTheme = localStorage.getItem("theme");
if (savedTheme === "dark") {
document.body.classList.add("dark-mode");
toggle.textContent = "Toggle Light Mode";
} else if (savedTheme === "light") {
document.body.classList.add("light-mode");
}

toggle.addEventListener("click", () => {
if (document.body.classList.contains("dark-mode")) {
document.body.classList.replace("dark-mode", "light-mode");
localStorage.setItem("theme", "light");
toggle.textContent = "Toggle Dark Mode";
} else {
document.body.classList.replace("light-mode", "dark-mode");
localStorage.setItem("theme", "dark");
toggle.textContent = "Toggle Light Mode";
}
});
};
</script>
</body>
</html>


    )
  );

  // clang-format on

  return resp;
}

HttpResponse
NodeServer::route_info (HttpRequest req)
{
  dbg ("inside '/info'");
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

  resp.add_body (node ? node->to_string () : JSON_RAW ({
    "message" : "Node is not available. Please don't send requests here"
  }));

  return resp;
}

HttpResponse
NodeServer::route_connect_to_chain (HttpRequest req)
{
  HttpResponse resp;
  dbg ("Inside '/connect'");

  if (!node)
    {
      resp.status_code = HttpStatusEnum::ServiceUnavailable;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.add_body (JSON_RAW ({
        "message" : "Node is not available. Please don't send requests here"
      }));

      return resp;
    }

  json_t jreq = json_t::from_string (req.body);

  if (!jreq.has_key ("bnt_url"))
    {
      resp.status_code = HttpStatusEnum::Unauthorized;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.add_body (JSON_RAW ({ "error" : "No URL specified" }));
      return resp;
    }

  std::string url = jreq["bnt_url"]->as_string ();

  size_t colon_pos = url.find (':');
  if (colon_pos == std::string::npos)
    {
      resp.status_code = HttpStatusEnum::BadRequest;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.add_body (JSON_RAW ({
        "error" : "Invalid node URL format. Expected format: host:port"
      }));
      return resp;
    }

  std::string host = url.substr (0, colon_pos);
  std::string port_str = url.substr (colon_pos + 1);

  int port;
  try
    {
      port = std::stoi (port_str);
    }
  catch (const std::exception &e)
    {
      resp.status_code = HttpStatusEnum::BadRequest;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.add_body (JSON_RAW ({ "error" : "Invalid port number" }));
      return resp;
    }

  json_t jc;
  J (jc["ns_url"])
      = std::string (BKRS_SERVER_URL) + ":" + std::to_string (get_port ());

  dbg ("jc: " << jc.to_string ());
  std::string urs;

  try
    {
      urs = fetch_POST (host, port, "/addnode", jc.to_string ());
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what () << '\n';
      resp.status_code = HttpStatusEnum::InternalServerError;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.add_body (JSON_RAW ({ "error" : "Internal Server Error" }));
      return resp;
    }

  dbg ("urs Response: " << urs);

  if (urs.find ("OK") != std::string::npos)
    node->set_bnt_url (url);
  else
    {
      resp.status_code = HttpStatusEnum::InternalServerError;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.add_body (std::string ("{") + urs.substr (urs.find ("\"error\"")));
      return resp;
    }

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Type: application/json");

  resp.add_body (JSON_RAW ({ "message" : "Connected successfully" }));
  return resp;
}

void
NodeServer::add_routes ()
{
  add_route ("/", { "GET", "POST" },
             [this] (HttpRequest req) { return this->route_top (req); });

  add_route ("/info", { "GET" },
             [this] (HttpRequest req) { return this->route_info (req); });

  add_route ("/connect", { "POST" }, [this] (HttpRequest req) {
    return this->route_connect_to_chain (req);
  });
}

void
NodeServer::set_node (Node *n)
{
  if (node != nullptr)
    {
      std::string url = node->get_ns_url ();
      node->set_ns_url ("");
      node = n;
      node->set_ns_url (url);
    }
  else
    {
      node = n;
      node->set_ns_url (std::string (BKRS_SERVER_URL) + ":"
                        + std::to_string (get_port ()));
    }
}
} // namespace rs::block
