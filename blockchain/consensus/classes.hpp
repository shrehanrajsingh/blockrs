#if !defined(CONSENSUS_CLASSES_H)
#define CONSENSUS_CLASSES_H

#include "../../header.hpp"
#include "../../json/json.hpp"

namespace rs::block
{
using namespace json;

enum class ConsensusTypeEnum
{
  ProofOfWork = 0,
};

/**
 * The flow of work in a
 * Consensus mechanism is as follows
 * function compute (data) {
 *      ...compute params from data
 *      while operate (params)["status"] == 0 {
 *          params = next(params);
 *      }
 * }
 */
class Consensus
{
protected:
  ConsensusTypeEnum type;

public:
  Consensus ();
  Consensus (ConsensusTypeEnum);

  /**
   * With each parameters evaluated
   * at a step, the parameters are
   * bundled up in a json object
   * and `operate` is called on it
   * The result can yield two options:
   * 1) operate was "successful", i.e., it computed the new parameters
   * which match the block properties
   * 2) operate was "unsuccessful", i.e., we need to compute new parameters
   * and test again
   */
  virtual json_t operate (json_t &) = 0;

  /**
   * All the parameters required
   * to compute new data which
   * satisfies the block properties
   * of the network are passed
   * as json and the required response
   * is also in json
   */
  virtual json_t compute (json_t &) = 0;

  /**
   * Compute new parameters
   * to operate on.
   */
  virtual json_t next (json_t &) = 0;

  ~Consensus ();
};
} // namespace rs::block

#endif // CONSENSUS_CLASSES_H
