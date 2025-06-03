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
  Wallet *wallet;

  bool is_mining = false;
  bool reject_mine
      = false; /* some other block mined before and transmitted
                  the new blocks so current mining execution should stop */

  /* node routes */
  /* we will call '/' top */
  _RURL ("/") REQ ("GET", "POST") ROUTE (top);
  _RURL ("/info") REQ ("GET") ROUTE (info);
  _RURL ("/connect") REQ ("POST") ROUTE (connect_to_chain);
  _RURL ("/mine") REQ ("GET") ROUTE (mine);

  /* fetch latest nodes from blockchain */
  _RURL ("/update") REQ ("GET") ROUTE (update);

  /* wallet info */
  _RURL ("/wallet") REQ ("GET") ROUTE (wallet);

  _RURL ("/wallet/sign") REQ ("POST") ROUTE (wallet_sign);

public:
  NodeServer ();
  NodeServer (Node *);
  NodeServer (Node *, Wallet *);
  ~NodeServer ();

  void add_routes () override;

  void set_node (Node *);
  void set_wallet (Wallet *);

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
