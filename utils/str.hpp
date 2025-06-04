#if !defined(STR_H)
#define STR_H

#pragma once

#include "vec.hpp"
#include <cstring>
#include <iostream>

namespace rs::util
{
class Str
{
private:
  Vec<char> v;

public:
  Str () : v (Vec<char> ()) {};
  ~Str () {};

  Str (const Str &rhs) : v (rhs.v) {}
  Str (Str &&rhs) noexcept : v (std::move (rhs.v)) {}
  Str (char rhs) { v.push_back (rhs); }
  Str (char *rhs)
  {
    size_t rl = strlen (rhs);
    for (size_t i = 0; i < rl; i++)
      v.push_back (rhs[i]);
  }
  //   Str (const Str &rhs) { v = std::move (rhs.v); }

  int find (char);
  int find (char, int);
  int find (char *);

  bool
  empty ()
  {
    return v.get_size () == 0;
  }

  Str substr (size_t, size_t);

  Str &
  operator= (const Str &rhs)
  {
    if (this != &rhs)
      v = rhs.v;
    return *this;
  }

  Str &
  operator= (Str &&rhs)
  {
    if (this != &rhs)
      v = std::move (rhs.v);
    return *this;
  }

  Str (const char *_s)
  {
    char *s = (char *)_s;
    while (*s)
      push_back (*s++);
  }

  Str &
  operator= (char *_S)
  {
    v.clear ();
    while (*_S)
      v.push_back (*_S++);

    return *this;
  }

  Str &
  operator= (const char *_S)
  {
    char *s = (char *)_S;
    v.clear ();
    while (*s)
      v.push_back (*s++);

    return *this;
  }

  size_t
  size () const
  {
    return v.get_size ();
  }

  void
  push_back (char c)
  {
    v.push_back (c);
  }

  char
  pop_back (void)
  {
    return v.pop_back ();
  }

  char &
  operator[] (size_t i)
  {
    return v[i];
  }

  const char
  operator[] (size_t i) const
  {
    return v[i];
  }

  char *
  c_str ()
  {
    char *p;
    char *s = p = new char[v.get_size () + 1];

    for (char c : v)
      *p++ = c;
    *p = '\0';

    return s;
  }

  char *
  get_internal_buffer ()
  {
    return v.get ();
  }

  Vec<char> &
  get_vec ()
  {
    return v;
  }

  friend std::ostream &
  operator<< (std::ostream &_Out, Str &rhs)
  {
    for (char c : rhs.v)
      _Out << c;

    return _Out;
  }

  Str
  operator+ (const Str &rhs)
  {
    Str res (*this);

    for (size_t i = 0; i < rhs.size (); i++)
      res.push_back (rhs[i]);

    return res;
  }

  void
  clear ()
  {
    v.clear ();
  }

  Str &trim ();

  friend Str operator+ (const Str &, const char);
  friend bool operator== (const Str &, const Str &);
  friend bool operator== (const Str &, char *);
  friend bool operator== (const Str &, char);
  friend Str &operator+= (Str &, const char);
  friend Str &operator+= (Str &, const char *);
  friend Str &operator+= (Str &, Str &);
};

// std::ostream &
// operator<< (std::ostream &_Out, Str &rhs)
// {
//   for (auto c : rhs.v)
//     _Out << c;

//   return _Out;
// }
}

#endif // STR_H
