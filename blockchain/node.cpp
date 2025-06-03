#include "node.hpp"
#include "../server/server.hpp"
#include "blocknetwork.hpp"
#include "transaction.hpp"

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
  J (j["ns_url"]) = ns_url;
  J (j["bnt_url"]) = bnt_url;

  std::vector<JsonObject *> bk;

  for (Block *b : blocks)
    {
      json_t *jb = new json_t;
      *jb = json_t::from_string (b->to_string ());

      bk.push_back (new JsonObject (jb));
    }

  J (j["blocks"]) = bk;

  return j.to_string ();
}

Node
Node::from_string (std::string jstr)
{
  json_t j = json_t::from_string (jstr);

  Node node;

  if (j.has_key ("type"))
    {
      node.type = static_cast<NodeTypeEnum> (J (j["type"]).as_integer ());
    }

  if (j.has_key ("ns_url"))
    {
      node.ns_url = J (j["ns_url"]).as_string ();
    }

  if (j.has_key ("bnt_url"))
    {
      node.bnt_url = J (j["bnt_url"]).as_string ();
    }

  if (j.has_key ("blocks"))
    {
      std::vector<JsonObject *> blocks_json = J (j["blocks"]).as_array ();

      for (JsonObject *block_json : blocks_json)
        {
          std::stringstream ss;
          ss << *block_json;

          dbg ("ss: " << ss.str ());
          Block *block = new Block;
          *block = Block::from_string (ss.str ());

          node.blocks.push_back (block);
        }
    }

  return node;
}

void
Node::fetch_blocks_from_chain ()
{
  std::string info_resp;

  try
    {
      info_resp = fetch (get_bnt_url (), "GET", "/info");
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what () << '\n';
      return;
    }

  dbg ("info_resp: " << info_resp);

  size_t bidx = info_resp.find ("\r\n\r\n");
  if (info_resp.find ("200 OK") > bidx)
    return;

  info_resp = info_resp.substr (bidx);
  dbg ("json_info_resp: " << info_resp);

  json_t jp = json_t::from_string (info_resp);

  if (!jp.has_key ("chain_info"))
    throw std::invalid_argument ("Missing parameter 'chain_info'");

  json_t *jci = J (jp["chain_info"]).as_object ();

  if (!jci->has_key ("blocks"))
    throw std::invalid_argument ("Missing parameter 'blocks'");

  std::vector<JsonObject *> jo = J ((*jci)["blocks"]).as_array ();

  for (JsonObject *&jv : jo)
    {
      Block *bp = new Block;

      std::stringstream ss;
      ss << *jv;

      dbg ("jv_str: " << ss.str ());

      *bp = Block::from_string (ss.str ());
      blocks.push_back (bp);
    }
}

int
Node::mine ()
{
  if (!get_bnt_url ().size ())
    return -1; /* blockchain has not been connected yet */
  if (!blocks.size ())
    return -1; /* no genesis block, likely a faulty blockchain */

  Block *&last_block = blocks.back ();
  dbg ("Inside node::mine()");

  json_t params;
  J (params["prev_hash"]) = last_block->hash ();
  J (params["difficulty_target"]) = int (last_block->header.difficulty_target);

  json_t resp = mech.compute (params);
  int nonce = resp["nonce"]->as_integer ();

  std::string tr_r = fetch (get_bnt_url (), "GET", "/transaction/all");
  size_t bidx = tr_r.find ("\r\n\r\n");

  std::vector<Transaction> tr_pending; /* we only need pending transactions */

  if (tr_r.find ("200 OK") > bidx)
    {
      /* error in response */
      /* skip */
    }
  else
    {
      tr_r = tr_r.substr (bidx + 1);
      json_t jtr = json_t::from_string (tr_r);

      if (jtr.has_key ("transaction_pending"))
        {
          std::vector<JsonObject *> arr
              = jtr["transaction_pending"]->as_array ();

          for (JsonObject *&i : arr)
            {
              std::stringstream ss;
              ss << *i;

              dbg ("tr_pending i: " << ss.str ());
              tr_pending.push_back (Transaction::from_string (ss.str ()));
            }
        }
    }

  Block nb;
  nb.header = (BlockHeader){ .difficulty_target
                             = last_block->header.difficulty_target,
                             .nonce = static_cast<size_t> (nonce),
                             .prev_hash = params["prev_hash"]->as_string (),
                             .timestamp = time (NULL),
                             .version = last_block->header.version };

  nb.transactions_list = tr_pending;
  int r = blocks.size (); /* index of newly added block */
  add_block (nb);
  dbg ("Inside node::mine(): added block");

  return r;
}

void
Node::add_block (Block &bk)
{
  Block *nb = new Block;
  *nb = bk;
  blocks.push_back (nb);

  blocks.back ()->header.timestamp = time (NULL);

  if (blocks.size () > 1)
    blocks.back ()->header.prev_hash = blocks[blocks.size () - 2]->hash ();
}

void
Node::add_block (Block &&bk)
{
  Block *nb = new Block;
  *nb = std::move (bk);
  blocks.push_back (nb);

  blocks.back ()->header.timestamp = time (NULL);

  if (blocks.size () > 1)
    blocks.back ()->header.prev_hash = blocks[blocks.size () - 2]->hash ();
}

} // namespace rs::block
