#include "bnserver.hpp"

namespace rs::block
{
BlocknetServer::BlocknetServer () : HttpServer () {}
BlocknetServer::~BlocknetServer ()
{
  if (fd != -1)
    this->close ();
}

void
BlocknetServer::set_network (BlockNetwork *s)
{
  blockchain = s;
}

void
BlocknetServer::add_node (Node *n)
{
  n->set_bnt_url (std::string (BKRS_SERVER_URL) + ":"
                  + std::to_string (get_port ()));

  nodes.push_back (n);
}

void
BlocknetServer::remove_node (size_t s)
{
  nodes.erase (nodes.begin () + s);
}

HttpResponse
BlocknetServer::route_bn_top (HttpRequest _Req)
{
  dbg ("inside '/'");

  HttpResponse resp;
  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  // clang-format off
    resp.body = HTML_RAW (
<html>
<head>
<title>Blockchain Network Server</title>
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
<body class="light-mode">
<div class="container">
<button class="theme-toggle" id="theme-toggle">Toggle Dark Mode</button>
<h1>BlockNetwork Server</h1>
<p>
Welcome to BlockNetwork Server. This is the first endpoint of your own blockchain network.
<br>
You can use this server to create and manage your own blockchain network.
<br>
<br>
<strong>Note:</strong> This is a template for the BlockNetwork server. You can customize it to suit your needs.
<br>
<strong>Important:</strong> This server is not intended for production use. It is meant for testing and development purposes only.
<br>
<strong>Warning:</strong> Do not use this server for any sensitive or confidential data. It is not secure and may expose your data to unauthorized access.
<br>
<strong>Disclaimer:</strong> The developer of this server is not responsible for any loss or damage caused by the use of this server. Use it at your own risk.
<br>
<strong>License:</strong> This server is licensed under the MIT License. You can use it for any purpose, but you must include the original license and copyright notice in any derivative works.
<br>
<strong>Contact:</strong> If you have any questions or feedback, please contact the developer at <a href="mailto:shrehanofficial@gmail.com">his email</a>
</p>
</div>

<script>
window.onload = () => {
const toggle = document.getElementById('theme-toggle');

const savedTheme = localStorage.getItem('theme');
if (savedTheme === 'dark') {
document.body.classList.add('dark-mode');
toggle.textContent = 'Toggle Light Mode';
} else if (savedTheme === 'light') {
document.body.classList.add('light-mode');
}

toggle.addEventListener('click', () => {
if (document.body.classList.contains('dark-mode')) {
document.body.classList.replace('dark-mode', 'light-mode');
localStorage.setItem('theme', 'light');
toggle.textContent = 'Toggle Dark Mode';
} else {
document.body.classList.replace('light-mode', 'dark-mode');
localStorage.setItem('theme', 'dark');
toggle.textContent = 'Toggle Light Mode';
}
});
};
</script>
</body>
</html>  
);
    // clang-format on
    return resp;
}

HttpResponse
BlocknetServer::route_bn_nodes (HttpRequest _Req)
{
  dbg ("inside '/nodes'");

  HttpResponse resp;
  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  HttpHeader r = parse_header ("Content-Type: application/json");
  resp.head_map[r.name] = r;

  json_t j;
  std::vector<JsonObject *> nd;

  for (Node *&i : nodes)
    {
      json_t *jp = new json_t;
      *jp = json_t::from_string (i->to_string ());
      nd.push_back (new JsonObject (jp));
    }

  J (j["nodes"]) = nd;
  resp.body = j.to_string ();

  return resp;
}

HttpResponse
BlocknetServer::route_bn_add_node (HttpRequest req)
{
  HttpResponse resp;
  json_t jreq = json_t::from_string (req.body);

  if (!jreq.has_key ("url")) /* ns_url */
    {
      resp.status_code = HttpStatusEnum::BadRequest;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = JSON_RAW ({ "error" : "Missing parameter 'url'" });
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      return resp;
    }

  std::string url = jreq["url"]->as_string ();
  size_t colon_pos = url.rfind (":");

  for (Node *&n : nodes)
    {
      if (n->get_ns_url () == url)
        {
          resp.status_code = HttpStatusEnum::BadRequest;
          resp.status_message = get_status_message (resp.status_code);

          resp.body = JSON_RAW ({ "error" : "Node already exists" });
          resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
              "Content-Length: " + std::to_string (resp.body.size ()));
          resp.head_map[HttpHeaderEnum::ContentType]
              = parse_header ("Content-Type: application/json");

          return resp;
        }
    }

  if (colon_pos == std::string::npos)
    {
      resp.status_code = HttpStatusEnum::BadRequest;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = JSON_RAW ({ "error" : "Invalid parameter 'url'" });
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      return resp;
    }

  std::string host = url.substr (0, colon_pos);
  std::string port_str = url.substr (colon_pos + 1);

  dbg ("host: " << host << "\nport_str: " << port_str);

  int port;
  try
    {
      port = std::stoi (port_str);
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what () << '\n';
      resp.status_code = HttpStatusEnum::InternalServerError;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = JSON_RAW ({ "error" : "Internal Server Error" });
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      return resp;
    }

  std::string info_resp = fetch_GET (host, port, "/info");

  if (info_resp.find ("200 OK") == std::string::npos)
    {
      resp.status_code = HttpStatusEnum::InternalServerError;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = JSON_RAW ({ "error" : "<blocknetwork>/addnode crashed" });
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      return resp;
    }

  info_resp = info_resp.substr (info_resp.find ("\r\n\r\n") + 1);
  json_t jinfo = json_t::from_string (info_resp);

  Node *n = new Node (NodeTypeEnum (jinfo["type"]->as_integer ()),
                      jinfo["ns_url"]->as_string ());

  add_node (n);

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  resp.body = JSON_RAW ({ "message" : "Added successfully" });
  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));
  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");

  return resp;
}

void
BlocknetServer::add_routes ()
{
  add_route ("/", { "GET", "POST", "HEAD" },
             [this] (HttpRequest req) { return this->route_bn_top (req); });

  add_route ("/nodes", { "GET", "HEAD" },
             [this] (HttpRequest req) { return this->route_bn_nodes (req); });

  add_route ("/addnode", { "POST" }, [this] (HttpRequest req) {
    return this->route_bn_add_node (req);
  });
}
} // namespace rs::block
