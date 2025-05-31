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
class BlocknetServer : public _Server_Base
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

public:
  BlocknetServer ();
  ~BlocknetServer ();

  void setup_socket () override;
  void handle_client (int) override;
  void run () override;
  void close () override;

  int get_port () override;
  void set_port (int) override;
  int get_max_clients () override;
  void set_max_clients (int) override;

  inline std::vector<RouteInfo> &
  get_routes ()
  {
    return routes;
  }

  void add_routes ();

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

  void add_route (std::string _Path, std::vector<std::string> _AllowedRoutes,
                  std::function<HttpResponse (HttpRequest)> _Callback);
};
} // namespace rs::block

#endif // BNSERVER_H
