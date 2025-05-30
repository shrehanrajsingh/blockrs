#if !defined(TRANSACTION_H)
#define TRANSACTION_H

#include "../header.hpp"
#include "../json/json.hpp"

#define GAS_PRICE_DEFAULT 50

namespace rs::block
{
enum class TransactionStatusEnum
{
  Success = 0,
  Pending = 1,
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
 */
struct Transaction
{
  std::string tr_hash;
  TransactionStatusEnum status;
  size_t block_num;
  std::time_t timestamp;
  std::string from;
  std::string to;
  size_t value;
  std::string symbol;
  size_t gas_used;
  size_t gas_price = GAS_PRICE_DEFAULT;
  size_t tr_fee;
  size_t nonce;
  std::string input_data;
  std::string signature;

  Transaction () {}

  void hash ();
  std::string to_string ();
  static Transaction from_string (std::string); /* json representation */

  ~Transaction () {}
};
} // namespace rs::block

#endif // TRANSACTION_H
