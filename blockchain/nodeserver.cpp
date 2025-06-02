#include "nodeserver.hpp"
#include "blocknetwork.hpp"

namespace rs::block
{
NodeServer::NodeServer () : HttpServer (), node (nullptr) {}
NodeServer::NodeServer (Node *n) : HttpServer (), node (n) {}
NodeServer::~NodeServer ()
{
  if (fd != -1)
    this->close ();
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
<title>Node Server</title>
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
  json_t jreq = json_t::from_string (req.body);

  if (!jreq.has_key ("url")) /* bnt_url */
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
  size_t colon_pos = url.find_last_of (':');

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

  std::string url_pass
      = BKRS_SERVER_URL + std::string (":") + std::to_string (get_port ());
  std::string addnode_info
      = fetch_POST (host, port, "/addnode", "{\"url\": \"" + url_pass + "\"}");

  dbg ("connect response: " << addnode_info);

  if (addnode_info.find ("200 OK") == std::string::npos)
    {
      resp.status_code = HttpStatusEnum::InternalServerError;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = addnode_info.substr (addnode_info.find ("\r\n\r\n") + 1);
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      return resp;
    }

  this->node->set_bnt_url (url);

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  resp.body = JSON_RAW ({ "message" : "Connected successfully" });
  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));
  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");

  return resp;
}

HttpResponse
NodeServer::route_mine (HttpRequest req)
{
  dbg ("inside '/mine'");
  HttpResponse resp;

  if (is_mining)
    {
      resp.status_code = HttpStatusEnum::AlreadyReported;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = JSON_RAW ({ "message" : "Node is already mining" });
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      return resp;
    }

  is_mining = true;
  std::thread t ([this] () {
    node->mine ();
    is_mining = false;
  });

  t.detach ();

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  resp.body = JSON_RAW ({ "message" : "Deployed miner thread." });
  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));
  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");

  return resp;
}

HttpResponse
NodeServer::route_update (HttpRequest req)
{
  HttpResponse resp;

  if (node == nullptr)
    {
      resp.status_code = HttpStatusEnum::InternalServerError;
      resp.status_message = get_status_message (resp.status_code);

      resp.body
          = JSON_RAW ({ "error" : "NodeServer is not connected to a Node" });

      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));

      return resp;
    }

  dbg ("req.body: " << req.body);
  json_t ji = json_t::from_string (req.body);

  dbg ("ji: " << ji.to_string ());

  // if (!ji.has_key ("chain_info"))
  //   {
  //     resp.status_code = HttpStatusEnum::InternalServerError;
  //     resp.status_message = get_status_message (resp.status_code);

  //     resp.body = JSON_RAW ({ "error" : "Missing parameter 'chain_info'" });

  //     resp.head_map[HttpHeaderEnum::ContentType]
  //         = parse_header ("Content-Type: application/json");
  //     resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
  //         "Content-Length: " + std::to_string (resp.body.size ()));

  //     return resp;
  //   }

  // json_t *jci = ji["chain_info"]->as_object ();

  // if (!jci->has_key ("blocks"))
  if (!ji.has_key ("blocks"))
    {
      resp.status_code = HttpStatusEnum::InternalServerError;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = JSON_RAW ({ "error" : "Missing parameter 'blocks'" });

      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));

      return resp;
    }

  const std::vector<JsonObject *> &blks = ji["blocks"]->as_array ();
  std::vector<Block *> nchain;

  dbg ("blk_size: " << blks.size ());
  for (JsonObject *i : blks)
    {
      std::stringstream ss;
      ss << *i;

      Block *bp = new Block;
      *bp = Block::from_string (ss.str ());

      nchain.push_back (bp);
    }

  node->get_blocks () = nchain;

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  resp.body = JSON_RAW ({ "message" : "Chain updated successfully" });

  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");
  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));

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

  add_route ("/mine", { "GET" },
             [this] (HttpRequest req) { return this->route_mine (req); });

  add_route ("/update", { "GET" },
             [this] (HttpRequest req) { return this->route_update (req); });
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
