#include "blocknetwork.hpp"
#include "transaction.hpp"

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

BlockNetwork::BlockNetwork () : owner ("") {}
BlockNetwork::BlockNetwork (Transaction &t) : owner ("")
{
  create_genesis_block (t);
}

BlockNetwork::BlockNetwork (Wallet &w) : owner ("")
{
  create_genesis_block_from_owner (w);
}

void
BlockNetwork::create_genesis_block (Transaction &t)
{
  if (get_chain ().size ())
    throw std::runtime_error ("Genesis block can only be created once");

  owner = t.from;
  add_transaction (t);

  add_block ((Block){ .header = (BlockHeader){ .difficulty_target
                                               = BK_DEFAULT_DIFFICULTY_TARGET,
                                               .nonce = 10,
                                               .prev_hash = "",
                                               .timestamp = time (NULL),
                                               .version = "0.0.1" } });

  if (!get_chain ().size ())
    throw std::runtime_error ("Error creating Genesis block");
}

void
BlockNetwork::create_genesis_block_from_owner (Wallet &w)
{
  std::string owner = "0x" + to_hex (w.get_address ().data (), 20);
  Transaction t = (Transaction){
    .from = owner,
    .to = owner,
    .gas_price = GAS_PRICE_DEFAULT,
    .gas_used = 2100.0,
    .input_data = "",
    .nonce = 10,
    .symbol = "RS",
    .timestamp = time (NULL),
    .tr_fee = 2100.0f * GAS_PRICE_DEFAULT / 1000.0f,
    .value = 210000 /* send 210k tokens */
  };

  w.sign_transaction (t);
  create_genesis_block (t);
}

void
BlockNetwork::add_block (Block &rhs)
{
  chain.push_back (rhs);
  ch_map[chain.back ().hash ()] = chain.size () - 1;

  verify_transactions ();
  chain.back ().transactions_list = get_pending_transactions ();
  chain.back ().header.timestamp = time (NULL);

  for (Transaction &t : chain.back ().transactions_list)
    t.status = TransactionStatusEnum::Success;

  get_pending_transactions ().clear ();

  if (chain.size () > 1)
    chain.back ().header.prev_hash = chain[chain.size () - 2].hash ();
}

void
BlockNetwork::add_block (Block &&rhs)
{
  chain.push_back (std::move (rhs));
  ch_map[chain.back ().hash ()] = chain.size () - 1;

  verify_transactions ();
  chain.back ().transactions_list = get_pending_transactions ();

  for (Transaction &t : chain.back ().transactions_list)
    {
      t.status = TransactionStatusEnum::Success;
      t.block_num = chain.size () - 1;
    }

  get_pending_transactions ().clear ();

  if (chain.size () > 1)
    chain.back ().header.prev_hash = chain[chain.size () - 2].hash ();
}

void
BlockNetwork::add_transaction (Transaction &t)
{
  try
    {
      transactions_pending.push_back (t);
      Transaction &bk = transactions_pending.back ();
      bk.status = TransactionStatusEnum::Pending;
      bk.block_num = chain.size () - 1;
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what () << '\n';
      return;
    }
}

void
BlockNetwork::add_transaction (Transaction &&t)
{
  try
    {
      transactions_pending.push_back (std::move (t));
      Transaction &bk = transactions_pending.back ();
      bk.status = TransactionStatusEnum::Pending;
      bk.block_num = chain.size () - 1;
    }
  catch (const std::exception &e)
    {
      std::cerr << e.what () << '\n';
      return;
    }
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
      dbg ("transactions_list_i: " << i.to_string ());
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
  std::vector<JsonObject *> trns_pending;
  std::vector<JsonObject *> trns_rej;

  for (Block &block : chain)
    {
      json_t *jo = new json_t;
      *jo = json_t::from_string (block.to_string ());
      blocks.push_back (new JsonObject (jo));
    }

  for (Transaction &t : transactions_pending)
    {
      json_t *jo = new json_t;
      *jo = json_t::from_string (t.to_string ());
      trns_pending.push_back (new JsonObject (jo));
    }

  for (Transaction &t : transactions_rejected)
    {
      json_t *jo = new json_t;
      *jo = json_t::from_string (t.to_string ());
      trns_rej.push_back (new JsonObject (jo));
    }

  dbg ("trns_rej_size: " << trns_rej.size ());
  J (j["blocks"]) = blocks;
  J (j["owner"]) = owner;
  J (j["transactions_pending"]) = trns_pending;
  J (j["transactions_rejected"]) = trns_rej;

  std::string result = j.to_string ();
  dbg ("BlockNetwork::to_string(): " << result);
  return result;
}

void
BlockNetwork::verify_transactions ()
{
  std::vector<size_t> rej_idxs;
  size_t i = 0;

  for (Transaction &t : transactions_pending)
    {
      std::string str_sign = t.to_string_sign ();

      uint8_t h[32];
      const char *ss = str_sign.c_str ();
      SHA256 (reinterpret_cast<const uint8_t *> (ss), str_sign.size (), h);
      std::vector<uint8_t> h_vec (h, h + 32);

      std::vector<uint8_t> pk;
      if (!recover_public_key (h_vec, from_hex (t.signature), pk))
        {
          /* invalid transaction */
          rej_idxs.push_back (i);
          i++;
          continue;
        }

      if (!Wallet::verify_with_pubkey (pk, t.signature, str_sign))
        {
          /* invalud transaction */
          rej_idxs.push_back (i);
          i++;
          continue;
        }

      i++;
    }

  std::vector<Transaction> tr_new;

  for (size_t i = 0; i < transactions_pending.size (); i++)
    {
      if (std::find (rej_idxs.begin (), rej_idxs.end (), i) != rej_idxs.end ())
        {
          transactions_pending[i].status = TransactionStatusEnum::Rejected;
          get_rejected_transactions ().push_back (transactions_pending[i]);
        }
      else
        tr_new.push_back (transactions_pending[i]);
    }

  transactions_pending = tr_new;
}

bool
BlockNetwork::valid_chain ()
{
  for (size_t i = 1; i < chain.size (); i++)
    {
      if (chain[i].header.prev_hash != chain[i - 1].hash ())
        return false;
    }

  return true;
}

Block
Block::from_string (std::string str)
{
  json_t jp = json_t::from_string (str);
  Block b;

  b.header.difficulty_target = size_t (jp["difficulty_target"]->as_integer ());
  b.header.nonce = size_t (jp["nonce"]->as_integer ());
  b.header.prev_hash = jp["prev_hash"]->as_string ();
  b.header.timestamp = time_t (jp["timestamp"]->as_integer ());
  b.header.version = jp["version"]->as_string ();

  if (jp.has_key ("transactions"))
    {
      std::vector<JsonObject *> jar = jp["transactions"]->as_array ();

      /**
       * A known bug in json representation
       * is that sometimes empty arrays are
       * represented as [0] (an actual 0 with array length of 1)
       * We need to detect that.
       */
      if (jar.size ())
        {
          if (jar[0]->get_type () != JsonType::Object)
            goto end;
        }

      for (JsonObject *jv : jp["transactions"]->as_array ())
        {
          std::stringstream ss;
          ss << *jv;

          b.transactions_list.push_back (Transaction::from_string (ss.str ()));
        }
    }

end:
  return b;
}

} // namespace rs::block
