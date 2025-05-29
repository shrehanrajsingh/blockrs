#include "str.hpp"

namespace rs::util
{
Str
operator+ (const Str &lhs, const char rhs)
{
  Str res (lhs);
  res.push_back (rhs);

  return res;
}

bool
operator== (const Str &lhs, const Str &rhs)
{
  if (lhs.size () != rhs.size ())
    return false;

  for (size_t i = 0; i < lhs.size (); i++)
    if (lhs[i] != rhs[i])
      return false;

  return true;
}

bool
operator== (const Str &lhs, char *rhs)
{
  size_t rl = strlen (rhs);
  if (lhs.size () != rl)
    return false;

  size_t i = 0;
  while (*rhs)
    {
      if (i >= lhs.size ())
        return false;

      if (lhs[i] != *rhs)
        return false;

      i++;
      rhs++;
    }

  if (*rhs == '\0' && i == rl)
    return true;

  return false;
}

bool
operator== (const Str &lhs, char rhs)
{
  if (lhs.size () != 1)
    return false;

  return lhs[0] == rhs;
}

Str &
operator+= (Str &lhs, const char c)
{
  lhs.push_back (c);
  return lhs;
}

Str &
operator+= (Str &lhs, const char *s)
{
  size_t sl = strlen (s);

  for (size_t i = 0; i < sl; i++)
    lhs.push_back (s[i]);

  return lhs;
}

Str &
operator+= (Str &lhs, Str &s)
{
  for (char c : s.v)
    lhs.push_back (c);

  return lhs;
}

int
Str::find (char c)
{
  for (int i = 0; i < v.get_size (); i++)
    {
      if (c == v[i])
        return i;
    }

  return -1;
}

int
Str::find (char c, int st)
{
  for (int i = st; i < v.get_size (); i++)
    {
      if (c == v[i])
        return i;
    }

  return -1;
}

int
Str::find (char *c)
{
  size_t c_len = strlen (c);

  if (c_len == 0)
    return 0;

  if (c_len > v.get_size ())
    return -1;

  for (int i = 0; i <= v.get_size () - c_len; i++)
    {
      bool found = true;

      for (size_t j = 0; j < c_len; j++)
        {
          if (v[i + j] != c[j])
            {
              found = false;
              break;
            }
        }

      if (found)
        return i;
    }

  return -1;
}

Str
Str::substr (size_t st, size_t end)
{
  if (st >= v.get_size () || st >= end)
    {
      return Str ();
    }

  if (end > v.get_size ())
    {
      end = v.get_size ();
    }

  Str result = "";
  for (size_t i = st; i < end; i++)
    {
      result.push_back (v[i]);
    }
  return result;
}

Str &
Str::trim ()
{
  if (empty ())
    return *this;

  while (size () > 0
         && (v[size () - 1] == '\n' || v[size () - 1] == '\r'
             || v[size () - 1] == '\t' || v[size () - 1] == ' '))
    pop_back ();

  if (empty ())
    return *this;

  size_t i = 0, j = 0;
  while (i < size ()
         && (v[i] == '\n' || v[i] == '\t' || v[i] == '\r' || v[i] == ' '))
    i++;

  if (i == size ())
    {
      clear ();
      return *this;
    }

  while (i < size ())
    {
      v[j] = v[i];
      i++;
      j++;
    }

  while (size () > j)
    pop_back ();

  return *this;
}

} // namespace rs::util
