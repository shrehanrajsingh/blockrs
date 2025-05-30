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

  TEST (3);

  return 0;
}