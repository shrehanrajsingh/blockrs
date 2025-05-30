#if !defined(WALLET_H)
#define WALLET_H

#include "../header.hpp"
#include "../lib/tiny_sha3/sha3.h"
#include <secp256k1.h>

#define KECCAK_256_RATE 136
#define KECCAK_256_OUTPUT_SIZE 32

namespace rs::block
{
void keccak_256 (const uint8_t *, size_t, uint8_t *);
std::string to_hex (const uint8_t *, size_t);
std::vector<uint8_t> from_hex (const std::string &);
std::array<uint8_t, 32> to_fixed_key_pk (const std::string &);
std::array<uint8_t, 20> to_fixed_key_ad (const std::string &);

class Wallet
{
  std::array<uint8_t, 32> private_key;
  std::vector<uint8_t> public_key;
  std::array<uint8_t, 20> address;

  void _gen_private_key ();
  void _gen_public_key ();
  void _gen_address ();

public:
  Wallet () { setup (); }
  Wallet (std::string _PrivateKey, std::string _PublicKey,
          std::string _Address);

  void setup ();

  inline std::vector<uint8_t> &
  get_public_key ()
  {
    return public_key;
  }

  inline std::vector<uint8_t>
  get_public_key () const
  {
    return public_key;
  }

  inline std::array<uint8_t, 32> &
  get_private_key ()
  {
    return private_key;
  }

  inline std::array<uint8_t, 20> &
  get_address ()
  {
    return address;
  }

  std::string sign (std::string);
  static bool verify (const Wallet &_Wallet, std::string _Signature,
                      std::string _Message);

  ~Wallet () {}
};
} // namespace rs::block

#endif // WALLET_H
