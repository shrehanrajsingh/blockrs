#include "blocknetwork.hpp"

namespace rs::block
{
std::string
Block::hash ()
{
  std::string bls = this->to_string ();
  dbg ("Block::hash(): " << bls);

  uint8_t h[32];
  const char *s = bls.c_str ();

  SHA256 (reinterpret_cast<const uint8_t *> (s), bls.size (), h);
  return to_hex (h, 32);
}

BlockNetwork::BlockNetwork () { /* create genesis block */ }

void
BlockNetwork::add_block (Block &rhs)
{
  chain.push_back (rhs);
  ch_map[chain.back ().hash ()] = chain.size () - 1;

  if (chain.size () > 1)
    chain.back ().header.prev_hash = chain[chain.size () - 2].hash ();
}

void
BlockNetwork::add_block (Block &&rhs)
{
  chain.push_back (std::move (rhs));
  ch_map[chain.back ().hash ()] = chain.size () - 1;

  if (chain.size () > 1)
    chain.back ().header.prev_hash = chain[chain.size () - 2].hash ();
}

Block &
BlockNetwork::get_block (std::string s)
{
  if (ch_map.find (s) != ch_map.end ())
    throw std::invalid_argument ("Block with given hash does not exist");

  return chain[ch_map[s]];
}

Block &
BlockNetwork::get_block (size_t s)
{
  if (s >= chain.size ())
    throw std::out_of_range ("Index greater than block size");

  return chain[s];
}

std::string
BlockHeader::to_string ()
{
  json_t j;
  J (j["version"]) = version;
  J (j["prev_hash"]) = prev_hash;
  J (j["timestamp"]) = int (timestamp);
  J (j["difficulty_target"]) = int (difficulty_target);
  J (j["nonce"]) = int (nonce);

  std::string r = j.to_string ();
  dbg ("BlockHeader::to_string(): " << r);

  return r;
}

std::string
Block::to_string ()
{
  std::string bls = header.to_string ();

  json_t j = json_t::from_string (bls);

  std::vector<JsonObject *> jar;

  for (Transaction &i : transactions_list)
    {
      json_t *jo = new json_t;
      *jo = json_t::from_string (i.to_string ());
      jar.push_back (new JsonObject (jo));
    }

  J (j["transactions"]) = jar;

  bls = j.to_string ();

  dbg ("Block::to_string(): " << bls);
  return bls;
}

std::string
BlockNetwork::to_string ()
{
  json_t j;
  std::vector<JsonObject *> blocks;

  for (Block &block : chain)
    {
      json_t *jo = new json_t;
      *jo = json_t::from_string (block.to_string ());
      blocks.push_back (new JsonObject (jo));
    }

  J (j["blocks"]) = blocks;
  std::string result = j.to_string ();
  dbg ("BlockNetwork::to_string(): " << result);
  return result;
}

} // namespace rs::block
