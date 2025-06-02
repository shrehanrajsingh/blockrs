#include "classes.hpp"

namespace rs::block
{
Consensus::Consensus () : type (ConsensusTypeEnum::ProofOfWork) {}
Consensus::Consensus (ConsensusTypeEnum _Type) : type (_Type) {}

Consensus::~Consensus () {}
} // namespace rs::block
