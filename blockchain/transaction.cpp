#include "transaction.hpp"
#include "wallet.hpp"

namespace rs::block
{
void
Transaction::hash ()
{
  using namespace json;
  json_t j;
  J (j["status"]) = int (status);
  J (j["block_num"]) = int (block_num);
  J (j["timestamp"]) = int (timestamp);
  J (j["from"]) = from;
  J (j["to"]) = to;
  J (j["value"]) = value;
  J (j["symbol"]) = symbol;
  J (j["gas_used"]) = gas_used;
  J (j["gas_price"]) = gas_price;
  J (j["tr_fee"]) = tr_fee;
  J (j["nonce"]) = int (nonce);
  J (j["input_data"]) = input_data;
  J (j["signature"]) = signature;
  J (j["is_cbt"]) = is_coinbase_transaction;

  std::string strj = j.to_string ();
  const char *smsg = strj.c_str ();
  uint8_t h[32];

  SHA256 (reinterpret_cast<const uint8_t *> (smsg), strj.size (), h);
  tr_hash = to_hex (h, 32);
}

std::string
Transaction::to_string ()
{
  using namespace json;
  json_t j;
  J (j["status"]) = int (status);
  J (j["block_num"]) = int (block_num);
  J (j["timestamp"]) = int (timestamp);
  J (j["from"]) = from;
  J (j["to"]) = to;
  J (j["value"]) = int (value);
  J (j["symbol"]) = symbol;
  J (j["gas_used"]) = int (gas_used);
  J (j["gas_price"]) = int (gas_price);
  J (j["tr_fee"]) = int (tr_fee);
  J (j["nonce"]) = int (nonce);
  J (j["input_data"]) = input_data;
  J (j["signature"]) = signature;
  J (j["tr_hash"]) = tr_hash;
  J (j["is_cbt"]) = is_coinbase_transaction;

  return j.to_string ();
}

Transaction
Transaction::from_string (std::string s)
{
  using namespace json;
  json_t j = json_t::from_string (s);

  Transaction t;
  t.block_num = j["block_num"]->as_integer ();
  t.status = TransactionStatusEnum (j["status"]->as_integer ());
  t.timestamp = j["timestamp"]->as_integer ();
  t.from = j["from"]->as_string ();
  t.to = j["to"]->as_string ();
  t.value = j["value"]->as_integer ();
  t.symbol = j["symbol"]->as_string ();
  t.gas_used = j["gas_used"]->as_integer ();
  t.gas_price = j["gas_price"]->as_integer ();
  t.tr_fee = j["tr_fee"]->as_integer ();
  t.nonce = j["nonce"]->as_integer ();
  t.input_data = j["input_data"]->as_string ();
  t.signature = j["signature"]->as_string ();
  t.tr_hash = j["tr_hash"]->as_string ();
  t.is_coinbase_transaction = j["is_cbt"]->as_boolean ();

  return t;
}

std::string
Transaction::to_string_sign ()
{
  json::json_t j;
  J (j["nonce"]) = int (nonce);
  J (j["to"]) = to;
  J (j["value"]) = value;
  J (j["gas_fee"]) = gas_price;
  J (j["data"]) = input_data;

  std::string js = j.to_string ();
  dbg ("Transaction::to_string_sign(): " << js);

  return js;
}
} // namespace rs::block
