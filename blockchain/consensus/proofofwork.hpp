#if !defined(PROOFOFWORK_H)
#define PROOFOFWORK_H

#include "classes.hpp"

namespace rs::block
{
using namespace json;

/**
 * For this implementation of ProofOfWork
 * we will use Bitcoin's PoW algorithm
 * which involves taking the hash of the
 * previous block and concatenating a
 * number `nonce` to it.
 * If the hash of the resultant string
 * starts with `difficulty_target` number of zeroes
 * then the nonce is valid
 */
class ProofOfWork : public Consensus
{
public:
  ProofOfWork ();
  ~ProofOfWork ();

  json_t operate (json_t &) override;
  json_t compute (json_t &) override;
  json_t next (json_t &) override;
};
} // namespace rs::block

#endif // PROOFOFWORK_H
