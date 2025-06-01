#include "walletserver.hpp"

namespace rs::block
{
WalletServer::WalletServer () : HttpServer () {}

WalletServer::~WalletServer ()
{
  if (fd != -1)
    close ();
}

HttpResponse
WalletServer::route_ws_top (HttpRequest req)
{
  HttpResponse resp;

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  //   resp.head_map[HttpHeaderEnum::ContentType]
  //       = parse_header ("Content-Type: application/html");

  // clang-format off
    resp.body = HTML_RAW (
<html>
<head>
<title>Wallet Server</title>
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
<h1>Wallet Server</h1>
<p>
Welcome to Wallet Server. This server helps you generate and manage
cryptocurrency wallets.
<br />
You can use this server to create public/private key pairs and wallet
addresses for your blockchain.
<br />
<br />
<strong>Note:</strong> This is a template for the Wallet Server. You can
customize it to suit your needs.
<br />
<strong>Important:</strong> This server is not intended for production
use. It is meant for testing and development purposes only.
<br />
<strong>Warning:</strong> Do not use this server for any sensitive or
confidential data. It is not secure and may expose your wallet keys to
unauthorized access.
<br />
<strong>Disclaimer:</strong> The developer of this server is not
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

);

    // clang-format on
    resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
        "Content-Length: " + std::to_string (resp.body.size ()));
    return resp;
}

HttpResponse
WalletServer::route_ws_info (HttpRequest req)
{
  HttpResponse resp;

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  json_t jw = json_t::from_string (w.to_string ());
  J (jw["private_key"]) = "<HIDDEN>";

  resp.body = jw.to_string ();

  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));
  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");

  return resp;
}

HttpResponse
WalletServer::route_ws_sign (HttpRequest req)
{
  dbg ("wallet/sign reqbody: " << req.body);
  json_t j = json_t::from_string (req.body);

  HttpResponse resp;

  if (!j.has_key ("message"))
    {
      resp.status_code = HttpStatusEnum::NotFound;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.body = JSON_RAW ({ "error" : "Missing parameter 'signature'" });

      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));

      return resp;
    }

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);
  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");

  resp.body = "{\"sign\": \"" + w.sign (j["message"]->as_string ()) + "\"}";
  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));

  return resp;
}

HttpResponse
WalletServer::route_ws_verify (HttpRequest req)
{
  json_t j = json_t::from_string (req.body);

  HttpResponse resp;

  if (!j.has_key ("signature"))
    {
      resp.status_code = HttpStatusEnum::NotFound;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.body = JSON_RAW ({ "error" : "Missing parameter 'signature'" });
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));

      return resp;
    }

  if (!j.has_key ("message"))
    {
      resp.status_code = HttpStatusEnum::NotFound;
      resp.status_message = get_status_message (resp.status_code);
      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      resp.body = JSON_RAW ({ "error" : "Missing parameter 'message'" });
      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));

      return resp;
    }

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);
  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");

  resp.body = "{\"status\": \""
              + (Wallet::verify (w, j["signature"]->as_string (),
                                 j["message"]->as_string ())
                     ? std::string ("1")
                     : std::string ("0"))
              + "\"}";
  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));

  return resp;
}

void
WalletServer::add_routes ()
{
  this->add_route ("/", { "GET" }, [this] (HttpRequest req) {
    return this->route_ws_top (req);
  });

  this->add_route ("/info", { "GET" }, [this] (HttpRequest req) {
    return this->route_ws_info (req);
  });

  this->add_route ("/sign", { "POST" }, [this] (HttpRequest req) {
    return this->route_ws_sign (req);
  });

  this->add_route ("/verify", { "POST" }, [this] (HttpRequest req) {
    return this->route_ws_verify (req);
  });
}

void
WalletServer::set_wallet (Wallet wl)
{
  w = wl;
}
} // namespace rs::block
