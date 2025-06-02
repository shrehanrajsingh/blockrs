#include "proofofwork.hpp"
#include "../wallet.hpp"

namespace rs::block
{
ProofOfWork::ProofOfWork () : Consensus () {}
ProofOfWork::~ProofOfWork () {}

json_t
ProofOfWork::compute (json_t &data)
{
  if (!data.has_key ("prev_hash") || !data.has_key ("difficulty_target"))
    throw std::invalid_argument (
        "Please provide 'prev_hash' and 'difficulty_target'");

  int nonce = 0;
  std::string ph = data["prev_hash"]->as_string ();
  int dt = data["difficulty_target"]->as_integer ();
  std::string pref (dt, '0');

  std::string cur;
  uint8_t h[32];
  cur = ph + std::to_string (nonce);

  SHA256 (reinterpret_cast<const uint8_t *> (cur.c_str ()), cur.size (), h);

  size_t num_0;
  while (1)
    {
      /**
       * For performance we will omit
       * the use of operate and next
       * since both require working with
       * json which is resource intensive.
       * For algorithms involving the
       * use of a lot of variables,
       * it becomes better to define and
       * use operate and next
       */
      /* next */
      num_0 = 0;
      while (num_0 < 32 && h[num_0] == '0')
        num_0++;

      dbg ("num_0: " << num_0 << "\tdt: " << dt
                     << "\nhash: " << to_hex (h, 32));

      /* operate */
      if (num_0 == dt)
        break;

      nonce++;
      cur = ph + std::to_string (nonce);

      SHA256 (reinterpret_cast<const uint8_t *> (cur.c_str ()), cur.size (),
              h);
    }

  json_t res;
  J (res["nonce"]) = nonce;
  J (res["hash"]) = to_hex (h, 32);

  return res;
}

json_t
ProofOfWork::operate (json_t &data)
{
  return json_t{};
}

json_t
ProofOfWork::next (json_t &data)
{
  return json_t{};
}

} // namespace rs::block
