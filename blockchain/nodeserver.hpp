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
class NodeServer : public _Server_Base
{
private:
  struct sockaddr_in address;
  int addrlen = 0;
  std::vector<RouteInfo> routes;
  Node *node;

  /* node routes */
  /* we will call '/' top */
  ROUTE (top) _RURL ("/") REQ ("GET", "POST");
  ROUTE (info) _RURL ("/info") REQ ("GET");

public:
  NodeServer ();
  NodeServer (Node *);
  ~NodeServer ();

  void setup_socket () override;
  void handle_client (int) override;
  void run () override;
  void close () override;

  int get_port () override;
  void set_port (int) override;
  int get_max_clients () override;
  void set_max_clients (int) override;

  void add_routes ();

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

  void add_route (std::string _Path, std::vector<std::string> _AllowedRoutes,
                  std::function<HttpResponse (HttpRequest)> _Callback);
};
} // namespace rs::block

#endif // NODESERVER_H
