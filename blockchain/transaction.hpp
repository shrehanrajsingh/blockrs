#if !defined(TRANSACTION_H)
#define TRANSACTION_H

#include "../header.hpp"
#include "../json/json.hpp"

#define GAS_PRICE_DEFAULT 50.0f

namespace rs::block
{
enum class TransactionStatusEnum
{
  Success = 0,
  Pending = 1,
  Rejected = 2,
};

/**
 * @class Transaction
 * @brief Represents a blockchain transaction
 *
 * This class encapsulates the details of a blockchain transaction on a
 * distributed ledger. It stores all necessary transaction data including
 * identification, parties involved, economic details, and status information.
 *
 * @details
 * The class maintains the following information:
 * - tr_hash: Unique transaction identifier/hash
 * - status: Current transaction status (from TransactionStatusEnum)
 * - block_num: Block number containing this transaction
 * - timestamp: Time when the transaction was created/processed
 * - from: Sender's address
 * - to: Recipient's address
 * - value: Amount of cryptocurrency being transferred
 * - symbol: Currency symbol/token identifier
 * - gas_used: Amount of computational resources used
 * - gas_price: Price per unit of gas
 * - tr_fee: Total transaction fee
 * - nonce: Sequence number for transactions from sender's address
 * - input_data: Additional data for the transaction (e.g., smart contract
 * - signature: Signature of the wallet that signed the transaction
 * calls)
 * - is_coinbase_transaction: A transaction which transfers assets to
 *    a node for adding a block
 *
 * In my model of Blockchain, I have implemented an "owner" entity which
 * stores all the funds in the blockchain (capped currency) and whenever
 * a node adds a block, certain amount of funds are transferred from the
 * "owner" to the node's wallet
 */
struct Transaction
{
  std::string tr_hash = "";
  TransactionStatusEnum status = TransactionStatusEnum::Pending;
  size_t block_num = 0;
  std::time_t timestamp = 0;
  std::string from = "";
  std::string to = "";
  float value = 0.0f;
  std::string symbol = "";
  float gas_used = 0.0f;
  float gas_price = GAS_PRICE_DEFAULT;
  float tr_fee = 0.0f;
  size_t nonce = 0;
  std::string input_data = "";
  std::string signature = "";
  bool is_coinbase_transaction = false;

  //   Transaction () = default;
  //   Transaction (Transaction &) = default;
  //   Transaction (Transaction &&) = default;

  void hash ();
  std::string to_string ();
  std::string to_string_sign (); /* json of data used for signing */
  static Transaction from_string (std::string); /* json representation */

  ~Transaction () {}
};
} // namespace rs::block

#endif // TRANSACTION_H
