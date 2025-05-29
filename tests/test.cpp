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

  json_t j ({ { "name", "Shrehan" }, { "age", 19 }, { "dob", 190706 } });

  std::cout << j["name"] << std::endl << j["age"] << std::endl << j["dob"];
}

int
main (int argc, char const *argv[])
{
  using namespace rs::block;
  using namespace rs::util;

  TEST (2);

  return 0;
}