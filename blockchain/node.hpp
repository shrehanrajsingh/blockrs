#if !defined(NODE_H)
#define NODE_H

#include "../header.hpp"
#include "../json/json.hpp"
#include "transaction.hpp"
#include "wallet.hpp"

namespace rs::block
{
enum class NodeTypeEnum
{
  Light = 0,
  Full = 1,
  Miner = 2,
  Authority = 3,
};

class Node
{
  NodeTypeEnum type;
  std::string ns_url;  /* url of nodeserver */
  std::string bnt_url; /* url of blockchainnetwork server */

public:
  Node () : type (NodeTypeEnum::Full) {}
  Node (NodeTypeEnum _Type) : type (_Type) {}
  Node (NodeTypeEnum _Type, std::string _NsURL) : type (_Type), ns_url (_NsURL)
  {
  }
  Node (NodeTypeEnum _Type, std::string _NsURL, std::string _BntURL)
      : type (_Type), ns_url (_NsURL), bnt_url (_BntURL)
  {
  }

  inline NodeTypeEnum &
  get_type ()
  {
    return type;
  }

  inline const NodeTypeEnum
  get_type () const
  {
    return type;
  }

  inline std::string &
  get_ns_url ()
  {
    return ns_url;
  }

  inline const std::string
  get_ns_url () const
  {
    return ns_url;
  }

  inline std::string &
  get_bnt_url ()
  {
    return bnt_url;
  }

  inline const std::string
  get_bnt_url () const
  {
    return bnt_url;
  }

  void
  set_ns_url (std::string _Url)
  {
    ns_url = _Url;
  }

  void
  set_bnt_url (std::string _Url)
  {
    bnt_url = _Url;
  }

  virtual std::string to_string ();

  ~Node () {}
};

class FullNode : public Node
{
public:
  FullNode () : Node (NodeTypeEnum::Full) {}
  FullNode (std::string _Url) : Node (NodeTypeEnum::Full, _Url) {}

  ~FullNode () {}
};

class MinerNode : public Node
{

public:
  MinerNode () : Node (NodeTypeEnum::Miner) {}
  MinerNode (std::string _Url) : Node (NodeTypeEnum::Miner, _Url) {}

  ~MinerNode () {}
};

class AuthorityNode : public Node
{

public:
  AuthorityNode () : Node (NodeTypeEnum::Authority) {}
  AuthorityNode (std::string _Url) : Node (NodeTypeEnum::Authority, _Url) {}

  ~AuthorityNode () {}
};

} // namespace rs::block

#endif // NODE_H
