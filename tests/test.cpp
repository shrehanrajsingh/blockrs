#include <blockrs.hpp>

int
main (int argc, char const *argv[])
{
  using namespace rs::block;
  using namespace rs::util;

  //   Server s;
  //   s.set_port (8000);
  //   s.run ();

  HttpServer s;
  s.set_port (8000);
  s.run ();

  return 0;
}