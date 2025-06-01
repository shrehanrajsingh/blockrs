#if !defined(BNSERVER_H)
#define BNSERVER_H

#include "../header.hpp"
#include "../json/json.hpp"
#include "blocknetwork.hpp"
#include "node.hpp"
#include "transaction.hpp"
#include "wallet.hpp"

namespace rs::block
{
/**
 * Derived from HttpServer
 */
class BlocknetServer : public HttpServer
{
  struct sockaddr_in address;
  int addrlen = 0;
  std::vector<RouteInfo> routes;
  BlockNetwork *blockchain;
  std::vector<Node *> nodes;

  /* warm server welcome */
  _RURL ("/") REQ ("GET", "POST", "HEAD") ROUTE (bn_top);

  /* list all nodes */
  _RURL ("/nodes") REQ ("GET", "HEAD") ROUTE (bn_nodes);

  /* add a node */
  _RURL ("/addnode") REQ ("POST") ROUTE (bn_add_node);

public:
  BlocknetServer ();
  ~BlocknetServer ();

  inline std::vector<RouteInfo> &
  get_routes ()
  {
    return routes;
  }

  void add_routes () override;

  void set_network (BlockNetwork *);

  inline BlockNetwork *&
  get_network ()
  {
    return blockchain;
  }

  void add_node (Node *);
  void remove_node (size_t);

  inline std::vector<Node *> &
  get_nodes ()
  {
    return nodes;
  }
};
} // namespace rs::block

#endif // BNSERVER_H
