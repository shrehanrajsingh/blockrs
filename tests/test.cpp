#include <blockrs.hpp>

#define TEST(X) test##X ()

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

  ns.run ();
}

int
main (int argc, char const *argv[])
{
  using namespace rs::block;
  using namespace rs::util;

  TEST (6);

  return 0;
}