#include "wallet.hpp"

namespace rs::block
{
void
keccak_256 (const uint8_t *input, size_t in_len, uint8_t *out)
{
  uint64_t state[25] = { 0 };

  size_t i, j;
  for (i = 0; i < in_len / KECCAK_256_RATE; i++)
    {
      for (j = 0; j < KECCAK_256_RATE / 8; j++)
        {
          uint64_t lane = 0;
          memcpy (&lane, input + i * KECCAK_256_RATE + j * 8, 8);
          state[j] ^= lane;
        }
      sha3_keccakf (state);
    }

  uint8_t temp[KECCAK_256_RATE] = { 0 };
  size_t rem = in_len % KECCAK_256_RATE;
  memcpy (temp, input + i * KECCAK_256_RATE, rem);

  temp[rem] = 0x01;
  temp[KECCAK_256_RATE - 1] |= 0x80;

  for (j = 0; j < KECCAK_256_RATE / 8; j++)
    {
      uint64_t lane = 0;
      memcpy (&lane, temp + j * 8, 8);
      state[j] ^= lane;
    }

  sha3_keccakf (state);
  memcpy (out, state, KECCAK_256_OUTPUT_SIZE);
}

std::string
to_hex (const uint8_t *data, size_t len)
{
  std::ostringstream oss;
  for (size_t i = 0; i < len; i++)
    oss << std::hex << std::setw (2) << std::setfill ('0') << (int)data[i];

  return oss.str ();
}

void
Wallet::_gen_private_key ()
{
  std::random_device rd;
  std::mt19937_64 eng (rd ());
  std::uniform_int_distribution<uint64_t> distr;

  std::array<uint8_t, 32> key{};
  for (int i = 0; i < 4; ++i)
    {
      uint64_t r = distr (eng);
      std::memcpy (key.data () + i * 8, &r, 8);
    }

  private_key = key;
}

void
Wallet::_gen_public_key ()
{
  secp256k1_context *ctx = secp256k1_context_create (SECP256K1_CONTEXT_SIGN);
  secp256k1_pubkey pubkey;

  if (!secp256k1_ec_pubkey_create (ctx, &pubkey, private_key.data ()))
    throw std::runtime_error ("Invalid private key");

  std::vector<uint8_t> output (65);
  size_t ol = output.size ();
  secp256k1_ec_pubkey_serialize (ctx, output.data (), &ol, &pubkey,
                                 SECP256K1_EC_UNCOMPRESSED);

  secp256k1_context_destroy (ctx);
  public_key = output;
}

void
Wallet::_gen_address ()
{
  const uint8_t *raw = public_key.data () + 1;

  uint8_t hash[32];
  keccak_256 (raw, 64, hash);

  memcpy (address.data (), hash + 12, 20);
}

void
Wallet::setup ()
{
  /* generate private key */
  _gen_private_key ();

  /* generate public key */
  _gen_public_key ();

  /* generate address */
  _gen_address ();
}

std::vector<uint8_t>
from_hex (const std::string &hex)
{
  std::string c_hex;

  if (hex.rfind ("0x", 0) == 0 || hex.rfind ("0X", 0) == 0)
    c_hex = hex.substr (2);
  else
    c_hex = hex;

  if (c_hex.length () % 2 != 0)
    throw std::invalid_argument ("Hex string must have even length");

  std::vector<uint8_t> bytes;
  bytes.reserve (c_hex.length () / 2);

  for (size_t i = 0; i < c_hex.length (); i += 2)
    {
      uint8_t byte = static_cast<uint8_t> (
          std::stoi (c_hex.substr (i, 2), nullptr, 16));
      bytes.push_back (byte);
    }

  return bytes;
}

std::array<uint8_t, 32>
to_fixed_key_pk (const std::string &hex)
{
  std::vector<uint8_t> vec = from_hex (hex);

  if (vec.size () != 32)
    throw std::invalid_argument ("Key must be 32 bytes");

  std::array<uint8_t, 32> arr;
  std::copy (vec.begin (), vec.end (), arr.begin ());
  return arr;
}

std::array<uint8_t, 20>
to_fixed_key_ad (const std::string &hex)
{
  std::vector<uint8_t> vec = from_hex (hex);

  if (vec.size () != 20)
    throw std::invalid_argument ("Key must be 20 bytes");

  std::array<uint8_t, 20> arr;
  std::copy (vec.begin (), vec.end (), arr.begin ());
  return arr;
}

Wallet::Wallet (std::string privk, std::string pubk, std::string addr)
{
  private_key = to_fixed_key_pk (privk);
  public_key = from_hex (pubk);
  address = to_fixed_key_ad (addr);
}

std::string
Wallet::sign (std::string s)
{
  secp256k1_context *ctx = secp256k1_context_create (SECP256K1_CONTEXT_SIGN);

  uint8_t hash[32];
  SHA256 (reinterpret_cast<const uint8_t *> (s.c_str ()), s.size (), hash);

  secp256k1_ecdsa_signature signature;
  if (!secp256k1_ecdsa_sign (ctx, &signature, hash, private_key.data (),
                             nullptr, nullptr))
    {
      std::cerr << "Signing failed" << std::endl;
      secp256k1_context_destroy (ctx);
      return "";
    }

  uint8_t sig[64];
  secp256k1_ecdsa_signature_serialize_compact (ctx, sig, &signature);

  std::string sigres = to_hex (sig, 32) + to_hex (sig + 32, 32);
  return sigres; /* r|s pair */
}

bool
Wallet::verify (const Wallet &w, std::string sig, std::string message)
{
  secp256k1_context *ctx = secp256k1_context_create (SECP256K1_CONTEXT_VERIFY);
  secp256k1_pubkey pubkey;

  if (!secp256k1_ec_pubkey_parse (ctx, &pubkey, w.get_public_key ().data (),
                                  w.get_public_key ().size ()))
    {
      std::cerr << "Failed to parse public key!" << std::endl;
      return false;
    }

  const char *msg = message.c_str ();
  uint8_t msg_hash[32];

  SHA256 (reinterpret_cast<const uint8_t *> (msg), strlen (msg), msg_hash);

  std::vector<uint8_t> sig_vec = from_hex (sig);

  secp256k1_ecdsa_signature signature;
  if (!secp256k1_ecdsa_signature_parse_compact (ctx, &signature,
                                                sig_vec.data ()))
    {
      std::cerr << "Invalid signature format" << std::endl;
      return false;
    }

  int v = secp256k1_ecdsa_verify (ctx, &signature, msg_hash, &pubkey);

  secp256k1_context_destroy (ctx);
  return v != 0;
}

} // namespace rs::block
