#if !defined(WALLETSERVER_H)
#define WALLETSERVER_H

#include "../header.hpp"
#include "../server/server.hpp"
#include "wallet.hpp"

namespace rs::block
{
class WalletServer : public HttpServer
{
  _RURL ("/") REQ ("GET") ROUTE (ws_top);
  _RURL ("/info") REQ ("GET") ROUTE (ws_info);
  _RURL ("/sign") REQ ("POST") ROUTE (ws_sign);
  _RURL ("/verify") REQ ("POST") ROUTE (ws_verify);

  Wallet w;

public:
  WalletServer ();
  ~WalletServer ();

  inline Wallet &
  get_wallet ()
  {
    return w;
  }

  void set_wallet (Wallet);
  void add_routes () override;
};
} // namespace rs::block

#endif // WALLETSERVER_H
