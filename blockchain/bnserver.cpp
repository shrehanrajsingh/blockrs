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

HttpResponse
BlocknetServer::route_bn_transaction_new (HttpRequest req)
{
  HttpResponse resp;

  switch (req.request_type.type)
    {
    case HttpRequestTypeEnum::Get:
      {
        resp.status_code = HttpStatusEnum::OK;
        resp.status_message = get_status_message (resp.status_code);
        // clang-format off
        {
            resp.body = HTML_RAW (
<!DOCTYPE html>
<html>
  <head>
    <title>Blockchain Transaction</title>
    <style>
      :root {
        --bg-color: #f8f8f8;
        --text-color: #333;
        --heading-color: #2c3e50;
        --link-color: #3498db;
        --input-bg: #fff;
        --input-border: #ddd;
      }

      @media (prefers-color-scheme: dark) {
        :root {
          --bg-color: #222;
          --text-color: #f0f0f0;
          --heading-color: #60a5fa;
          --link-color: #60a5fa;
          --input-bg: #333;
          --input-border: #444;
        }
      }

      body.light-mode {
        --bg-color: #f8f8f8;
        --text-color: #333;
        --heading-color: #2c3e50;
        --link-color: #3498db;
        --input-bg: #fff;
        --input-border: #ddd;
      }

      body.dark-mode {
        --bg-color: #222;
        --text-color: #f0f0f0;
        --heading-color: #60a5fa;
        --link-color: #60a5fa;
        --input-bg: #333;
        --input-border: #444;
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

      form {
        margin-top: 20px;
      }

      .form-group {
        margin-bottom: 15px;
      }

      label {
        display: block;
        margin-bottom: 5px;
        font-weight: bold;
      }

      input[type="text"],
      input[type="number"],
      input[type="url"],
      textarea {
        width: 100%;
        padding: 8px;
        border: 1px solid var(--input-border);
        border-radius: 4px;
        background-color: var(--input-bg);
        color: var(--text-color);
      }

      button[type="submit"] {
        background-color: var(--link-color);
        color: white;
        border: none;
        padding: 10px 15px;
        border-radius: 4px;
        cursor: pointer;
        font-weight: bold;
      }

      button[type="submit"]:hover {
        opacity: 0.9;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <button class="theme-toggle" id="theme-toggle">Toggle Dark Mode</button>
      <h1>New Blockchain Transaction</h1>

      <form method="post" action="">
        <div class="form-group">
          <label for="sender">Sender Address</label>
          <input
            type="text"
            id="sender"
            name="sender"
            required
            placeholder="Enter sender's blockchain address"
          />
        </div>

        <div class="form-group">
          <label for="recipient">Recipient Address</label>
          <input
            type="text"
            id="recipient"
            name="recipient"
            required
            placeholder="Enter recipient's blockchain address"
          />
        </div>

        <div class="form-group">
          <label for="amount">Amount</label>
          <input
            type="number"
            id="amount"
            name="amount"
            step="0.000001"
            min="0"
            required
            placeholder="Enter amount to transfer"
          />
        </div>

        <div class="form-group">
          <label for="fee">Transaction Fee</label>
          <input
            type="number"
            id="fee"
            name="fee"
            step="0.000001"
            min="0"
            required
            placeholder="Enter transaction fee"
          />
        </div>

        <div class="form-group">
          <label for="gas_limit">Gas Limit (optional)</label>
          <input
            type="number"
            id="gas_limit"
            name="gas_limit"
            min="0"
            placeholder="Enter gas limit"
          />
        </div>

        <div class="form-group">
          <label for="wallet_server_link">Enter WalletServer link (this will be used to sign the message)</label>
          <input
            type="url"
            id="wallet_server_link"
            name="wallet_server_link"
            placeholder="http://127.0.0.1:9000"
          />
        </div>

        <div class="form-group">
          <label for="memo">Memo/Message (optional)</label>
          <textarea
            id="memo"
            name="memo"
            rows="3"
            placeholder="Enter an optional message with this transaction"
          ></textarea>
        </div>

        <button type="submit">Submit Transaction</button>
      </form>

      <p>
        <strong>Note:</strong> This form creates a new blockchain transaction.
        <br />
        <strong>Warning:</strong> Always verify transaction details before
        submitting.
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
        }
        // clang-format on

        resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
            "Content-Length: " + std::to_string (resp.body.size ()));
      }
      break;

    case HttpRequestTypeEnum::Post:
      {
        resp.status_code = HttpStatusEnum::OK;
        resp.status_message = get_status_message (resp.status_code);

        std::map<std::string, std::string> form_data;
        std::string body = req.body;
        std::string delim = "&";
        size_t pos = 0;
        std::string token;

        while ((pos = body.find (delim)) != std::string::npos)
          {
            token = body.substr (0, pos);
            size_t equal_pos = token.find ("=");
            if (equal_pos != std::string::npos)
              {
                std::string key = token.substr (0, equal_pos);
                std::string value = token.substr (equal_pos + 1);

                std::string dec_val;
                for (size_t i = 0; i < value.length (); i++)
                  {
                    if (value[i] == '%' && i + 2 < value.length ())
                      {
                        int hex_val;
                        std::stringstream ss;
                        ss << std::hex << value.substr (i + 1, 2);
                        ss >> hex_val;
                        dec_val += static_cast<char> (hex_val);
                        i += 2;
                      }
                    else if (value[i] == '+')
                      {
                        dec_val += ' ';
                      }
                    else
                      {
                        dec_val += value[i];
                      }
                  }

                form_data[key] = dec_val;
              }
            body.erase (0, pos + delim.length ());
          }

        if (!body.empty ())
          {
            size_t equal_pos = body.find ("=");
            if (equal_pos != std::string::npos)
              {
                std::string key = body.substr (0, equal_pos);
                std::string value = body.substr (equal_pos + 1);

                std::string dec_val;
                for (size_t i = 0; i < value.length (); i++)
                  {
                    if (value[i] == '%' && i + 2 < value.length ())
                      {
                        int hex_val;
                        std::stringstream ss;
                        ss << std::hex << value.substr (i + 1, 2);
                        ss >> hex_val;
                        dec_val += static_cast<char> (hex_val);
                        i += 2;
                      }
                    else if (value[i] == '+')
                      {
                        dec_val += ' ';
                      }
                    else
                      {
                        dec_val += value[i];
                      }
                  }

                util::trim_string (key);
                form_data[key] = dec_val;
              }
          }

        json_t jresp;
        for (auto &i : form_data)
          {
            std::string ifstr = i.first;
            util::trim_string (ifstr);
            J (jresp[ifstr]) = i.second;
          }

        dbg ("jresp: " << jresp.to_string ());

        Transaction nt;

        try
          {
            nt = { .from = jresp["sender"]->as_string (),
                   .to = jresp["recipient"]->as_string (),
                   .gas_price = GAS_PRICE_DEFAULT,
                   .gas_used = 2100,
                   .input_data = jresp["memo"]->as_string (),
                   .tr_fee = std::stof (jresp["fee"]->as_string ()),
                   .value = std::stof (jresp["amount"]->as_string ()) };
          }
        catch (const std::exception &e)
          {
            std::cerr << e.what () << '\n';
            resp.status_code = HttpStatusEnum::InternalServerError;
            resp.status_message = get_status_message (resp.status_code);
            resp.body = JSON_RAW ({ "error" : "Internal Server Error" });

            HttpHeader content_type
                = parse_header ("Content-Type: application/json");
            HttpHeader content_length = parse_header (
                "Content-Length: " + std::to_string (resp.body.size ()));

            resp.head_map[HttpHeaderEnum::ContentType] = content_type;
            resp.head_map[HttpHeaderEnum::ContentLength] = content_length;

            return resp;
          }

        /* wallet sign response */
        std::string nts = nt.to_string_sign ();
        std::string escaped_nts;
        escaped_nts.reserve (nts.size () * 2);

        for (char c : nts)
          {
            if (c == '"')
              escaped_nts += "\\\"";
            else
              escaped_nts += c;
          }

        dbg ("escaped_nts: " << escaped_nts);
        std::string ws_resp
            = fetch (jresp["wallet_server_link"]->as_string (), "POST",
                     "/sign", "{\"message\": \"" + escaped_nts + "\"}");

        dbg ("ws_resp: " << ws_resp);

        if (ws_resp.find ("200 OK") == std::string::npos)
          {
            resp.status_code = HttpStatusEnum::InternalServerError;
            resp.status_message = get_status_message (resp.status_code);
            resp.body = JSON_RAW ({ "error" : "Internal Server Error" });

            HttpHeader content_type
                = parse_header ("Content-Type: application/json");
            HttpHeader content_length = parse_header (
                "Content-Length: " + std::to_string (resp.body.size ()));

            resp.head_map[HttpHeaderEnum::ContentType] = content_type;
            resp.head_map[HttpHeaderEnum::ContentLength] = content_length;

            return resp;
          }

        ws_resp = ws_resp.substr (ws_resp.find ("\r\n\r\n") + 1);

        json_t jws = json_t::from_string (ws_resp);

        if (!jws.has_key ("sign"))
          {
            resp.status_code = HttpStatusEnum::InternalServerError;
            resp.status_message = get_status_message (resp.status_code);
            resp.body = JSON_RAW ({ "error" : "Internal Server Error" });

            HttpHeader content_type
                = parse_header ("Content-Type: application/json");
            HttpHeader content_length = parse_header (
                "Content-Length: " + std::to_string (resp.body.size ()));

            resp.head_map[HttpHeaderEnum::ContentType] = content_type;
            resp.head_map[HttpHeaderEnum::ContentLength] = content_length;

            return resp;
          }

        std::string smsg = jws["sign"]->as_string ();
        nt.signature = smsg;
        nt.hash ();

        dbg ("nt.signature: " << smsg << "\nnt.hash: " << nt.tr_hash);

        if (blockchain == nullptr)
          {
            resp.status_code = HttpStatusEnum::BadGateway;
            resp.status_message = get_status_message (resp.status_code);
            resp.body = JSON_RAW ({
              "error" : "This BlocknetServer does not have a blockchain "
                        "associated with it"
            });

            HttpHeader content_type
                = parse_header ("Content-Type: application/json");
            HttpHeader content_length = parse_header (
                "Content-Length: " + std::to_string (resp.body.size ()));

            resp.head_map[HttpHeaderEnum::ContentType] = content_type;
            resp.head_map[HttpHeaderEnum::ContentLength] = content_length;

            return resp;
          }

        blockchain->add_transaction (nt);
        blockchain->verify_transactions ();

        resp.status_code = HttpStatusEnum::OK;
        resp.status_message = get_status_message (resp.status_code);
        resp.body = ("{\"message\": \"Transaction added successfully\", "
                     "\"status\": \"Pending\", \"hash\": \""
                     + nt.tr_hash + "\"}");

        HttpHeader content_type
            = parse_header ("Content-Type: application/json");
        HttpHeader content_length = parse_header (
            "Content-Length: " + std::to_string (resp.body.size ()));

        resp.head_map[HttpHeaderEnum::ContentType] = content_type;
        resp.head_map[HttpHeaderEnum::ContentLength] = content_length;
      }
      break;

    default:
      break;
    }

  return resp;
}

HttpResponse
BlocknetServer::route_bn_transaction_all (HttpRequest req)
{
  HttpResponse resp;
  json_t j;

  if (blockchain == nullptr)
    {
      resp.status_code = HttpStatusEnum::BadGateway;
      resp.status_message = get_status_message (resp.status_code);

      resp.body = JSON_RAW (
          { "error" : "Server is not connected to a blockchain network." });

      resp.head_map[HttpHeaderEnum::ContentLength] = parse_header (
          "Content-Length: " + std::to_string (resp.body.size ()));

      resp.head_map[HttpHeaderEnum::ContentType]
          = parse_header ("Content-Type: application/json");

      return resp;
    }

  std::vector<JsonObject *> tr_pending;
  std::vector<JsonObject *> tr_rej;

  for (Transaction &i : blockchain->get_pending_transactions ())
    {
      json_t *jv = new json_t;
      *jv = json_t::from_string (i.to_string ());

      tr_pending.push_back (new JsonObject (jv));
    }

  for (Transaction &i : blockchain->get_rejected_transactions ())
    {
      json_t *jv = new json_t;
      *jv = json_t::from_string (i.to_string ());

      tr_rej.push_back (new JsonObject (jv));
    }

  J (j["transaction_pending"]) = tr_pending;
  J (j["transaction_rejected"]) = tr_rej;

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);

  resp.body = j.to_string ();

  resp.head_map[HttpHeaderEnum::ContentLength]
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));

  resp.head_map[HttpHeaderEnum::ContentType]
      = parse_header ("Content-Type: application/json");

  return resp;
}

HttpResponse
BlocknetServer::route_bn_info (HttpRequest req)
{
  json_t jresp;

  json_t *jbk = new json_t;
  *jbk = json_t::from_string (blockchain->to_string ());

  J (jresp["chain_info"]) = jbk;
  std::vector<JsonObject *> node_info;

  for (Node *&i : nodes)
    {
      json_t *jo = new json_t;
      *jo = json_t::from_string (i->to_string ());
      node_info.push_back (new JsonObject (jo));
    }

  J (jresp["nodes"]) = node_info;

  HttpResponse resp;

  resp.status_code = HttpStatusEnum::OK;
  resp.status_message = get_status_message (resp.status_code);
  resp.body = jresp.to_string ();

  HttpHeader content_type = parse_header ("Content-Type: application/json");
  HttpHeader content_length
      = parse_header ("Content-Length: " + std::to_string (resp.body.size ()));

  resp.head_map[HttpHeaderEnum::ContentType] = content_type;
  resp.head_map[HttpHeaderEnum::ContentLength] = content_length;

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

  add_route ("/transaction/new", { "GET", "POST" }, [this] (HttpRequest req) {
    return this->route_bn_transaction_new (req);
  });

  add_route ("/transaction/all", { "GET", "POST" }, [this] (HttpRequest req) {
    return this->route_bn_transaction_all (req);
  });

  add_route ("/info", { "GET" },
             [this] (HttpRequest req) { return this->route_bn_info (req); });
}

void
BlocknetServer::run ()
{
  std::thread nft ([this] {
    while (1)
      {
        fetch_nodes ();

#ifdef _WIN32
        // TODO
#else
        sleep (BK_FETCHNODE_DELAY_S);
#endif
      }
  }); /* node fetch thread */

  nft.detach ();
  this->HttpServer::run ();
}

void
BlocknetServer::fetch_nodes ()
{
  if (blockchain == nullptr)
    return;

  Node *mlc_node = nullptr;

  for (Node *&i : nodes)
    {
      if (!i->get_bnt_url ().size ())
        continue;

      std::string info_r;

      int retries = 3;

      while (retries--)
        {
          try
            {
              info_r = fetch (i->get_ns_url (), "GET", "/info");
              break;
            }
          catch (const std::exception &e)
            {
              std::cerr << e.what () << '\n';
              if (!retries)
                {
                  dbg ("fetch <node>/info failed");
                  return;
                }
            }
        }

      dbg ("info_r: " << info_r);

      size_t bidx = info_r.find ("\r\n\r\n");

      if (info_r.find ("200 OK") > bidx)
        continue;

      info_r = info_r.substr (bidx + 1);

      for (size_t j = 0; j < i->get_blocks ().size (); j++)
        delete i->get_blocks ()[j];

      *i = Node::from_string (info_r);
    }

  bool saw_potential = false;
  for (Node *&i : nodes)
    {
      if (mlc_node == nullptr)
        {
          mlc_node = i;
          continue;
        }

      dbg ("blockchain->get_chain().size(): "
           << blockchain->get_chain ().size ()
           << "\tmlc_node->get_blocks().size(): "
           << mlc_node->get_blocks ().size ());

      if (i->get_blocks ().size () > mlc_node->get_blocks ().size ())
        {
          mlc_node = i;
          saw_potential = true;
        }
    }

  if (blockchain && mlc_node
      && mlc_node->get_blocks ().size () > blockchain->get_chain ().size ())
    saw_potential = true;

  /* replace main chain with longest chain in node */
  /* also send an update request to other nodes */
  if (saw_potential)
    {
      dbg ("saw_potential: 1");
      std::vector<Block> nchain;
      for (Block *&b : mlc_node->get_blocks ())
        {
          for (Transaction &i : b->transactions_list)
            i.status = TransactionStatusEnum::Success;

          nchain.push_back (*b);
        }

      blockchain->get_chain () = nchain;

      for (Node *&i : nodes)
        {
          if (i == mlc_node)
            {
              if (!nchain.size ())
                continue;

              /* add coinbase transaction */
              std::string winfo;

              try
                {
                  winfo = fetch (i->get_ns_url (), "GET", "/wallet");
                }
              catch (const std::exception &e)
                {
                  std::cerr << e.what () << '\n';
                  continue;
                }

              size_t bidx = winfo.find ("\r\n\r\n");

              if (winfo.find ("200 OK") > bidx)
                {
                  continue;
                }

              winfo = winfo.substr (bidx + 1);
              json_t jwi = json_t::from_string (winfo);

              if (!jwi.has_key ("address"))
                continue;

              std::string addr = jwi["address"]->as_string ();

              Transaction nt = (Transaction){
                .from = blockchain->owner,
                .to = addr,
                .gas_price = GAS_PRICE_DEFAULT,
                .gas_used = 2100,
                .input_data = "Transfer of currency value 21 &RS only as a "
                              "token for adding a block",
                .value = 21,
                .nonce = 19,
                .timestamp = time (NULL),
                .tr_fee = 105,
                .is_coinbase_transaction = true,
                .block_num = nchain.size () - 1,
              };

              std::string sign_tr;

              try
                {
                  std::string ds = nt.to_string_sign ();
                  sign_tr
                      = fetch (i->get_ns_url (), "POST", "/wallet/sign", ds);
                }
              catch (const std::exception &e)
                {
                  std::cerr << e.what () << '\n';
                  continue;
                }

              bidx = sign_tr.find ("\r\n\r\n");

              if (sign_tr.find ("200 OK") > bidx)
                continue;

              sign_tr = sign_tr.substr (bidx + 1);
              json_t jstr = json_t::from_string (sign_tr);

              if (!jstr.has_key ("sign"))
                continue;

              std::string sgmsg = jstr["sign"]->to_string ();
              nt.signature = sgmsg;
              nt.hash ();

              dbg ("pushing cbt");
              blockchain->get_chain ().back ().transactions_list.push_back (
                  nt);
            }
        }

      for (Node *&i : nodes)
        {
          try
            {
              /* transmit to all blocks an update signal */
              fetch (i->get_ns_url (), "GET", "/update",
                     blockchain->to_string ());
            }
          catch (const std::exception &e)
            {
              std::cerr << e.what () << '\n';
            }
        }
    }
}
} // namespace rs::block
