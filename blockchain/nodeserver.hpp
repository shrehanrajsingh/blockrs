#if !defined(NODESERVER_H)
#define NODESERVER_H

#include "../header.hpp"
#include "../server/server.hpp"
#include "node.hpp"
#include "transaction.hpp"
#include "wallet.hpp"

namespace rs::block
{
/**
 * Derived from HttpServer
 */
class NodeServer : public HttpServer
{
private:
  struct sockaddr_in address;
  int addrlen = 0;
  std::vector<RouteInfo> routes;
  Node *node;

  bool is_mining = false;

  /* node routes */
  /* we will call '/' top */
  _RURL ("/") REQ ("GET", "POST") ROUTE (top);
  _RURL ("/info") REQ ("GET") ROUTE (info);
  _RURL ("/connect") REQ ("POST") ROUTE (connect_to_chain);
  _RURL ("/mine") REQ ("GET") ROUTE (mine);

public:
  NodeServer ();
  NodeServer (Node *);
  ~NodeServer ();

  void add_routes () override;

  void set_node (Node *);

  inline Node *&
  get_node ()
  {
    return node;
  }

  inline std::vector<RouteInfo> &
  get_routes ()
  {
    return routes;
  }
};
} // namespace rs::block

#endif // NODESERVER_H
