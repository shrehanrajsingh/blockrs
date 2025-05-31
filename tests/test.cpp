#include <blockrs.hpp>

#define TEST(X) test##X ()

int gArgc;
char **gArgv;

void
test1 ()
{
  using namespace rs::block;
  using namespace rs::util;

  //   Server s;
  //   s.set_port (8000);
  //   s.run ();

  HttpServer s;
  s.set_port (8000);

  s.add_route ("/", { "GET", "POST" }, [] (HttpRequest hr) -> HttpResponse {
    std::string rb;

    switch (hr.request_type.type)
      {
      case HttpRequestTypeEnum::Get:
        {
          std::cout << "Inside route '/'::GET" << std::endl;
          rb = "<html>"
               "<head>"
               " <title>Server</title>"
               "</head>"
               "<body>"
               "  <h1>Hi</h1>"
               "</body>"
               "</html>";
        }
        break;

      case HttpRequestTypeEnum::Post:
        {
          std::cout << "Inside route '/'::POST" << std::endl;
        }
        break;

      default:
        break;
      }

    HttpResponse resp;

    resp.status_code = HttpStatusEnum::OK;
    resp.status_message = get_status_message (resp.status_code);

    resp.add_body (rb);
    return resp;
  });

  s.run ();
}

void
test2 ()
{
  using namespace rs::block;
  using namespace rs::json;

  json_t j ({ { "name", "Shrehan" },
              { "age", 19 },
              { "dob", 190706 },
              { "arr", std::vector{ new JsonObject (true), new JsonObject (10),
                                    new JsonObject (15.5f) } } });

  std::cout << *j["name"] << std::endl
            << *j["age"] << std::endl
            << *j["dob"] << std::endl
            << *j["arr"];
}

void
test3 ()
{
  using namespace rs::block;
  using namespace rs::json;

  json_t j = json_t::from_string ("{"
                                  "   \"name\": \"Shrehan\","
                                  "   \"age\": 19,"
                                  "   \"dob\": 190706,"
                                  "   \"arr\": [1, \"hello\", {"
                                  "         \"a\": true,"
                                  "         \"b\": null"
                                  "}, 4, 5]"
                                  "}");

  std::cout << j.to_string () << std::endl;
}

void
test4 ()
{
  using namespace rs::block;
  using namespace rs::json;

  Wallet w[10];

  for (int i = 0; i < 10; i++)
    {
      std::cout << "Private key: 0x"
                << to_hex (w[i].get_private_key ().data (), 32) << '\n';
      std::cout << "Public key: 0x"
                << to_hex (w[i].get_public_key ().data (),
                           w[i].get_public_key ().size ())
                << '\n';
      std::cout << "Address: 0x" << to_hex (w[i].get_address ().data (), 20)
                << '\n';
    }
}

void
test5 ()
{
  using namespace rs::block;
  using namespace rs::json;

  /**
   * Randomly generated wallet details
   * Taken from tests/TEST_WALLETS
   */
  Wallet w (
      "0x423df74376ecb588240106471ae521e576574893f6a5df013950ebfb733fd214",
      "0x04e057b29ab631df1d061118ba51966684fceb1de440a70a1cf3da789c0afda8f9f8a"
      "916c15063443b8787a85f8f9091bebaded3c0aef8fa8d46031a5c74da65fd",
      "0x9665a13fece00de1f60183822d55ac180484ac1d");

  std::string sig = w.sign ("Hello, World!");
  std::cout << "Signature: " << sig << std::endl;

  if (Wallet::verify (w, sig, "Hello, World!"))
    {
      std::cout << "Verified OK" << std::endl;
    }
  else
    {
      std::cout << "Verified ERR" << std::endl;
    }
}

void
test6 ()
{
  using namespace rs::block;
  using namespace rs::json;

  NodeServer ns;
  ns.set_port (8000);

  Node node;
  ns.set_node (&node);

  ns.run ();
}

void
test7 ()
{
  using namespace rs::block;
  using namespace rs::json;

  /**
   * Randomly generated wallet details
   * Taken from tests/TEST_WALLETS
   */
  Wallet w (
      "0x423df74376ecb588240106471ae521e576574893f6a5df013950ebfb733fd214",
      "0x04e057b29ab631df1d061118ba51966684fceb1de440a70a1cf3da789c0afda8f9f8a"
      "916c15063443b8787a85f8f9091bebaded3c0aef8fa8d46031a5c74da65fd",
      "0x9665a13fece00de1f60183822d55ac180484ac1d");

  /**
   * Randomly generated wallet details
   * Taken from tests/TEST_WALLETS
   */
  Wallet x (
      "0x86f423ccd53644c6a9b0c2bf16954dc6cd4d497ff90c0f28c456dcd48d7977a6",
      "0x0455dfe9408d150ed042eb682eeb117cad0b40a5c62514655eabb9767cb2d8f9e9d97"
      "664a68b912dad307532d2e586267b5be57748a4154ab75970c8bdd6378ceb",
      "0xac96ccdbefc43a9efe8430ec9aa403f48ae1ad40");

  /* send from w to x */
  Transaction t;
  t.block_num = 0; /* blockchain will update it */
  t.from = to_hex (w.get_address ().data (), 20);
  t.to = to_hex (x.get_address ().data (), 20);
  t.gas_price = GAS_PRICE_DEFAULT;
  t.gas_used = 2100;
  t.input_data = "";
  t.nonce = 1; /* incremented by one each time user sends a transaction */
  t.status = TransactionStatusEnum::Pending;
  t.symbol = "RS";
  t.timestamp = time (NULL);
  t.tr_fee = t.gas_price * t.gas_used / 1000.0f;
  t.value = 10.0f;

  w.sign_transaction (t);
  t.hash ();

  /* send from x to w */
  Transaction u;
  u.block_num = 0; /* blockchain will update it */
  u.from = to_hex (x.get_address ().data (), 20);
  u.to = to_hex (w.get_address ().data (), 20);
  u.gas_price = GAS_PRICE_DEFAULT;
  u.gas_used = 2100;
  u.input_data = "";
  u.nonce = 2; /* incremented by one each time user sends a transaction */
  u.status = TransactionStatusEnum::Pending;
  u.symbol = "RS";
  u.timestamp = time (NULL);
  u.tr_fee = u.gas_price * u.gas_used / 1000.0f;
  u.value = 10.0f;

  x.sign_transaction (u);
  u.hash ();

  /* try changing `x` here to `w` */
  // if (Wallet::verify (x, u.signature, u.to_string_sign ()))
  //   {
  //     std::cout << "Verified OK" << std::endl;
  //   }
  // else
  //   {
  //     std::cout << "Verified ERR" << std::endl;
  //   }

  BlockNetwork bn;
  bn.add_block ((Block){ .header = (BlockHeader){ .difficulty_target = 4,
                                                  .nonce = 10,
                                                  .prev_hash = "",
                                                  .timestamp = time (NULL),
                                                  .version = "0.0.1" },
                         .transactions_list = { t, u } });

  bn.to_string ();
}

void
test8 ()
{
  using namespace rs::block;
  using namespace rs::json;

  BlockNetwork bn;

  /**
   * Randomly generated wallet details
   * Taken from tests/TEST_WALLETS
   */
  Wallet w (
      "0x423df74376ecb588240106471ae521e576574893f6a5df013950ebfb733fd214",
      "0x04e057b29ab631df1d061118ba51966684fceb1de440a70a1cf3da789c0afda8f9f8a"
      "916c15063443b8787a85f8f9091bebaded3c0aef8fa8d46031a5c74da65fd",
      "0x9665a13fece00de1f60183822d55ac180484ac1d");

  Transaction t = { .from = "0x9665a13fece00de1f60183822d55ac180484ac1d",
                    .to = "0xac96ccdbefc43a9efe8430ec9aa403f48ae1ad40",
                    .gas_price = 50.0f,
                    .gas_used = 2100.0f,
                    .input_data = "",
                    .nonce = 10,
                    .status = TransactionStatusEnum::Pending,
                    .symbol = "RS",
                    .timestamp = time (NULL),
                    .tr_fee = (2100 * 50) / 1000.0f,
                    .value = 50 };

  w.sign_transaction (t);

  bn.add_transaction (t);
  bn.verify_transactions ();

  std::cout << bn.get_pending_transactions ().size () << '\t'
            << bn.get_rejected_transactions ().size () << std::endl;

  // std::string str_sign = t.to_string_sign ();

  // uint8_t h[32];
  // const char *ss = str_sign.c_str ();
  // SHA256 (reinterpret_cast<const uint8_t *> (ss), str_sign.size (), h);
  // std::vector<uint8_t> h_vec (h, h + 32);

  // std::vector<uint8_t> pk;
  // std::cout << recover_public_key (h_vec, from_hex (t.signature), pk);
}

void
test9 ()
{
  using namespace rs::block;
  using namespace rs::json;

  Wallet w (
      "0x423df74376ecb588240106471ae521e576574893f6a5df013950ebfb733fd214",
      "0x04e057b29ab631df1d061118ba51966684fceb1de440a70a1cf3da789c0afda8f9f8a"
      "916c15063443b8787a85f8f9091bebaded3c0aef8fa8d46031a5c74da65fd",
      "0x9665a13fece00de1f60183822d55ac180484ac1d");

  BlockNetwork bn (w);

  Transaction t = { .from = "0x9665a13fece00de1f60183822d55ac180484ac1d",
                    .to = "0xac96ccdbefc43a9efe8430ec9aa403f48ae1ad40",
                    .gas_price = 50.0f,
                    .gas_used = 2100.0f,
                    .input_data = "",
                    .nonce = 10,
                    .status = TransactionStatusEnum::Pending,
                    .symbol = "RS",
                    .timestamp = time (NULL),
                    .tr_fee = (2100 * 50) / 1000.0f,
                    .value = 50 };

  w.sign_transaction (t);
  bn.add_transaction (t);

  bn.add_block ((Block){
      .header = (BlockHeader){ .difficulty_target
                               = bn.get_block (0).header.difficulty_target,
                               .nonce = 10,
                               .timestamp = time (NULL),
                               .version = bn.get_block (0).header.version },
  });

  std::cout << bn.to_string ();

  if (bn.valid_chain ())
    std::cout << "Chain is valid!" << std::endl;
  else
    std::cout << "Chain is invalid!" << std::endl;
}

void
test10 ()
{
  using namespace rs::block;
  using namespace rs::json;

  Wallet w (
      "0x423df74376ecb588240106471ae521e576574893f6a5df013950ebfb733fd214",
      "0x04e057b29ab631df1d061118ba51966684fceb1de440a70a1cf3da789c0afda8f9f8a"
      "916c15063443b8787a85f8f9091bebaded3c0aef8fa8d46031a5c74da65fd",
      "0x9665a13fece00de1f60183822d55ac180484ac1d");

  BlockNetwork bn (w);

  Transaction t = { .from = "0x9665a13fece00de1f60183822d55ac180484ac1d",
                    .to = "0xac96ccdbefc43a9efe8430ec9aa403f48ae1ad40",
                    .gas_price = 50.0f,
                    .gas_used = 2100.0f,
                    .input_data = "",
                    .nonce = 10,
                    .status = TransactionStatusEnum::Pending,
                    .symbol = "RS",
                    .timestamp = time (NULL),
                    .tr_fee = (2100 * 50) / 1000.0f,
                    .value = 50 };

  w.sign_transaction (t);
  bn.add_transaction (t);

  bn.add_block ((Block){
      .header = (BlockHeader){ .difficulty_target
                               = bn.get_block (0).header.difficulty_target,
                               .nonce = 10,
                               .timestamp = time (NULL),
                               .version = bn.get_block (0).header.version },
  });

  std::cout << bn.to_string ();

  if (bn.valid_chain ())
    std::cout << "Chain is valid!" << std::endl;
  else
    std::cout << "Chain is invalid!" << std::endl;

  Node *n = new Node (NodeTypeEnum::Full, "", "");

  BlocknetServer bns;
  bns.set_port (8000);
  bns.add_node (n);

  bns.run ();
}

void
test11 ()
{
  using namespace rs::block;
  using namespace rs::json;

  rs::ArgumentParser parser (
      "blockrs",
      "A Program to Simulate Blockchain Networks, Nodes and Wallets Locally");

  /* rs::Argument &h =  */ parser.add_argument (
      "help", "h", "help", "Display this help message", false, true);

  rs::Argument &a = parser.add_argument (
      "server", "s", "server",
      "Type of Server you wish to create. [ chain | node | wallet ]", false,
      false, "chain");

  a.set_validator ([] (const std::string &value) {
    return value == "chain" || value == "node" || value == "wallet";
  });

  if (!parser.parse (gArgc, gArgv))
    {
      parser.print_help ();
      return;
    }

  if (parser.has ("help") || gArgc < 2)
    {
      parser.print_help ();
      return;
    }

  std::string server_type = parser.get ("server");

  if (server_type == "chain")
    {
      Wallet w (
          "0x423df74376ecb588240106471ae521e576574893f6a5df013950ebfb733fd214",
          "0x04e057b29ab631df1d061118ba51966684fceb1de440a70a1cf3da789c0afda8f"
          "9f8a"
          "916c15063443b8787a85f8f9091bebaded3c0aef8fa8d46031a5c74da65fd",
          "0x9665a13fece00de1f60183822d55ac180484ac1d");

      BlockNetwork bn (w);

      Transaction t = { .from = "0x9665a13fece00de1f60183822d55ac180484ac1d",
                        .to = "0xac96ccdbefc43a9efe8430ec9aa403f48ae1ad40",
                        .gas_price = 50.0f,
                        .gas_used = 2100.0f,
                        .input_data = "",
                        .nonce = 10,
                        .status = TransactionStatusEnum::Pending,
                        .symbol = "RS",
                        .timestamp = time (NULL),
                        .tr_fee = (2100 * 50) / 1000.0f,
                        .value = 50 };

      w.sign_transaction (t);
      bn.add_transaction (t);

      std::cout << bn.to_string ();

      if (bn.valid_chain ())
        std::cout << "Chain is valid!" << std::endl;
      else
        std::cout << "Chain is invalid!" << std::endl;

      // Node *n = new Node (NodeTypeEnum::Full, "", "");

      BlocknetServer bns;
      bns.set_port (8000);
      // bns.add_node (n);

      bns.run ();
    }

  else if (server_type == "node")
    {
      NodeServer ns;
      ns.set_port (8100);

      Node *n = new Node (NodeTypeEnum::Full, "", "");
      ns.set_node (n);

      ns.run ();
    }

  else if (server_type == "wallet")
    {
      dbg ("Wallet Server not implemented yet");
    }
}

int
main (int argc, char const *argv[])
{
  gArgc = argc;
  gArgv = (char **)argv;

  using namespace rs::block;
  using namespace rs::util;

  TEST (11);

  return 0;
}