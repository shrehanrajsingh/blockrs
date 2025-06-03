#if !defined(BLOCKNETWORK_H)
#define BLOCKNETWORK_H

#include "../header.hpp"
#include "../json/json.hpp"
#include "node.hpp"
#include "nodeserver.hpp"
#include "transaction.hpp"
#include "wallet.hpp"

#define BK_DEFAULT_DIFFICULTY_TARGET (2)

namespace rs::block
{
struct Block;
class BlocknetServer;
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
  static Block from_string (std::string _JsonStr);
};

class BlockNetwork
{
  std::vector<Block> chain;

  /**
   * Link hashes of blocks with their entry
   * in the chain for faster block accesses
   */
  std::map<std::string, size_t> ch_map;

  std::vector<Transaction> transactions_pending;
  std::vector<Transaction> transactions_rejected;

  std::string owner;

public:
  BlockNetwork ();
  BlockNetwork (Transaction &);
  BlockNetwork (Wallet &);

  /**
   * The Genesis block takes in a transaction
   * and registers the owner of the block,
   * which is the "from" address of the transation.
   * The transaction in the Genesis block is a
   * transfer of a specific amount of tokens from
   * the owner's address to the same address.
   */
  void create_genesis_block (Transaction &);
  void create_genesis_block_from_owner (Wallet &);

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

  inline std::vector<Transaction> &
  get_pending_transactions ()
  {
    return transactions_pending;
  }

  inline const std::vector<Transaction> &
  get_pending_transactions () const
  {
    return transactions_pending;
  }

  void
  set_pending_transactions (std::vector<Transaction> &tp)
  {
    transactions_pending = tp;
  }

  void
  set_pending_transactions (std::vector<Transaction> &&tp)
  {
    transactions_pending = std::move (tp);
  }

  inline std::vector<Transaction> &
  get_rejected_transactions ()
  {
    return transactions_rejected;
  }

  inline const std::vector<Transaction> &
  get_rejected_transactions () const
  {
    return transactions_rejected;
  }

  void
  set_rejected_transactions (std::vector<Transaction> &tp)
  {
    transactions_rejected = tp;
  }

  void
  set_rejected_transactions (std::vector<Transaction> &&tp)
  {
    transactions_rejected = std::move (tp);
  }

  void add_block (Block &);
  void add_block (Block &&);

  void add_transaction (Transaction &);
  void add_transaction (Transaction &&);

  void verify_transactions ();

  bool valid_chain ();

  Block &get_block (std::string); /* from hash */
  Block &get_block (size_t);      /* from block_num */

  std::string to_string ();

  friend class BlocknetServer;

  ~BlockNetwork () {}
};

} // namespace rs::block

#endif // BLOCKNETWORK_H
