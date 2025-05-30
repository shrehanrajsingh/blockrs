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

int
main (int argc, char const *argv[])
{
  using namespace rs::block;
  using namespace rs::util;

  TEST (1);

  return 0;
}