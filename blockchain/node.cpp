#include "node.hpp"

namespace rs::block
{
static const std::string ntrtable[]
    = { "Light", "Full", "Miner", "Authority" };

std::string
Node::to_string ()
{
  json::json_t j;

  J (j["type"]) = int (type);
  J (j["type_str"]) = ntrtable[int (type)];
  J (j["url"]) = ns_url;
  J (j["host"]) = bnt_url;

  return j.to_string ();
}
} // namespace rs::block
