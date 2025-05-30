#if !defined(BLOCKNETWORK_H)
#define BLOCKNETWORK_H

#include "../header.hpp"
#include "../json/json.hpp"
#include "node.hpp"
#include "nodeserver.hpp"
#include "transaction.hpp"
#include "wallet.hpp"

namespace rs::block
{
struct Block;
struct BlockHeader
{
  std::string version;
  std::string prev_hash;
  time_t timestamp;
  size_t difficulty_target;
  size_t nonce;

  std::string to_string ();
};

struct Block
{
  BlockHeader header;
  std::vector<Transaction> transactions_list;

  std::string hash ();
  std::string to_string ();
};

class BlockNetwork
{
  std::vector<Block> chain;

  /**
   * Link hashes of blocks with their entry
   * in the chain for faster block accesses
   */
  std::map<std::string, size_t> ch_map;

public:
  BlockNetwork ();

  inline std::vector<Block> &
  get_chain ()
  {
    return chain;
  }

  inline const std::vector<Block> &
  get_chain () const
  {
    return chain;
  }

  void add_block (Block &);
  void add_block (Block &&);

  Block &get_block (std::string); /* from hash */
  Block &get_block (size_t);      /* from block_num */

  std::string to_string ();

  ~BlockNetwork () {}
};

} // namespace rs::block

#endif // BLOCKNETWORK_H
