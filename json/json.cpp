#include "json.hpp"

namespace rs::json
{
JsonContext::JsonContext () {}

JsonContext::JsonContext (
    std::initializer_list<std::pair<std::string, JsonObject>> obj)
{
  for (auto &&i : obj)
    {
      ctx[i.first] = new JsonObject;
      *ctx[i.first] = i.second;
    }
}

JsonContext::~JsonContext ()
{
  //   for (auto it : ctx)
  //     {
  //       delete it.second;
  //     }
}

JsonObject *&
JsonContext::operator[] (std::string s)
{
  if (ctx.find (s) == ctx.end ())
    ctx[s] = new JsonObject;
  return ctx[s];
}

std::ostream &
operator<< (std::ostream &out, JsonObject &lhs)
{
  switch (lhs.type)
    {
    case JsonType::Array:
      {
        const std::vector<JsonObject *> &arr = lhs.as_array ();
        if (!arr.size ())
          {
            out << "[]";
          }
        else
          {
            out << "[";
            bool first = true;
            for (auto &item : arr)
              {
                if (!first)
                  out << ", ";
                first = false;
                out << *item;
              }
            out << "]";
          }
      }
      break;
    case JsonType::Boolean:
      {
        out << (lhs.as_boolean () ? "true" : "false");
      }
      break;
    case JsonType::Float:
      {
        out << lhs.as_float ();
      }
      break;
    case JsonType::Integer:
      {
        out << lhs.as_integer ();
      }
      break;
    case JsonType::NoType:
    case JsonType::Null:
      {
        out << "null";
      }
      break;
    case JsonType::Object:
      {
        out << "{";
        bool first = true;
        out << *(lhs.as_object ());
        out << "}";
      }
      break;
    case JsonType::String:
      {
        out << "\"";
        for (char c : lhs.as_string ())
          {
            switch (c)
              {
              case '\"':
                out << "\\\"";
                break;
              case '\\':
                out << "\\\\";
                break;
              case '\b':
                out << "\\b";
                break;
              case '\f':
                out << "\\f";
                break;
              case '\n':
                out << "\\n";
                break;
              case '\r':
                out << "\\r";
                break;
              case '\t':
                out << "\\t";
                break;
              default:
                if ('\x00' <= c && c <= '\x1f')
                  {
                    out << "\\u" << std::hex << std::setw (4)
                        << std::setfill ('0') << static_cast<int> (c);
                  }
                else
                  {
                    out << c;
                  }
              }
          }
        out << "\"";
      }
      break;
    default:
      {
        out << "null";
      }
      break;
    }
  return out;
}

std::ostream &
operator<< (std::ostream &out, JsonContext &rhs)
{
  bool first = true;
  for (const auto &[key, value] : rhs.ctx)
    {
      if (!first)
        out << ", ";
      first = false;
      out << "\"" << key << "\": " << *value;
    }
  return out;
}

JsonObject *
_parse_val (std::string &val)
{
  JsonObject *jo = new JsonObject;
  util::trim_string (val);

  if (val == "true" || val == "false")
    {
      *jo = val[0] == 't';
    }
  else if (val == "null")
    {
      *jo = JsonObject (nullptr);
    }
  else if (val[0] == '\"')
    {
      *jo = val.substr (1, val.size () - 2);
    }
  else if (val[0] == '{')
    {
      JsonContext *jobj = new JsonContext;
      std::string inner_content = val.substr (1, val.size () - 2);
      util::trim_string (inner_content);

      bool reading_key = true;
      bool reading_val = false;
      bool in_str = false;
      int gb = 0;

      std::string key;
      std::string value;

      size_t sl = inner_content.size ();

      for (size_t i = 0; i < sl; i++)
        {
          char &c = inner_content[i];

          switch (c)
            {
            case '{':
              if (!in_str)
                gb++;
              break;

            case '}':
              if (!in_str)
                gb--;
              break;

            case '\"':
              if (in_str)
                {
                  if (i > 0 && inner_content[i - 1] == '\\')
                    {
                      if (i > 1 && inner_content[i - 2] == '\\')
                        in_str = false;
                    }
                  else
                    in_str = false;
                }
              else
                in_str = true;
              break;

            case '[':
              if (!in_str)
                gb++;
              break;

            case ']':
              if (!in_str)
                gb--;
              break;

            case ':':
              if (!in_str && !gb)
                {
                  reading_key = false;
                  reading_val = true;
                  continue;
                }
              break;

            case ',':
              if (!in_str && !gb)
                {
                  util::trim_string (key);
                  key.erase (0, 1);
                  key.pop_back ();

                  try
                    {
                      (*jobj)[key] = _parse_val (value);
                    }
                  catch (const std::exception &e)
                    {
                      std::cerr << e.what () << '\n';
                    }

                  key = "";
                  value = "";
                  reading_key = true;
                  reading_val = false;
                  continue;
                }
              break;
            }

          if (reading_key)
            key += c;
          if (reading_val)
            value += c;
        }

      if (!key.empty ())
        {
          util::trim_string (key);
          key.erase (0, 1);
          key.pop_back ();

          try
            {
              (*jobj)[key] = _parse_val (value);
            }
          catch (const std::exception &e)
            {
              std::cerr << e.what () << '\n';
            }
        }

      *jo = jobj;
    }
  else if (val[0] == '[')
    {
      std::vector<JsonObject *> vls;
      std::string buf;

      bool in_str = false;
      int gb = 0;

      size_t vl = val.size ();

      for (size_t j = 1; j < vl; j++)
        {
          char &d = val[j];

          switch (d)
            {
            case '[':
            case '{':
              {
                if (!in_str)
                  gb++;
              }
              break;

            case ']':
              {
                if (j == vl - 1 && buf.size ())
                  {
                    if (!in_str && !gb)
                      {
                        vls.push_back (_parse_val (buf));
                        buf = "";
                      }
                  }
                else
                  {
                    if (!in_str)
                      gb--;
                  }
              }
              break;
            case '}':
              {
                if (!in_str)
                  gb--;
              }
              break;

            case '\"':
              {
                if (in_str)
                  {
                    if (val[j - 1] == '\\')
                      {
                        if (val[j - 2] == '\\')
                          in_str = false; /* \\" */
                      }
                    else
                      in_str = false;
                  }
                else
                  {
                    in_str = true;
                  }
              }
              break;

            case ',':
              {
                if (!in_str && !gb)
                  {
                    vls.push_back (_parse_val (buf));
                    buf = "";
                    continue;
                  }
              }
              break;

            default:
              break;
            }

          buf += d;
        }

      *jo = vls;
    }
  else
    {
      bool saw_digits = true;
      bool saw_dot = false;

      for (char &c : val)
        {
          if (c >= '0' && c <= '9')
            ;
          else
            {
              if (c == '.')
                {
                  assert (!saw_dot);
                  saw_dot = true;
                }
              else
                {
                  saw_digits = false;
                  break;
                }
            }
        }

      if (saw_digits)
        {
          if (saw_dot)
            {
              *jo = std::stof (val.c_str ());
            }
          else
            {
              *jo = std::stoi (val.c_str ());
            }
        }
      else
        {
          throw std::invalid_argument ("Invalid JSON value: " + val);
        }
    }

  return jo;
}

JsonContext
JsonContext::from_string (std::string s)
{
  util::trim_string (s);
  json_t r;

  size_t sl = s.size ();
  bool reading_key = true;
  bool reading_val = false;
  bool in_str = false;
  int gb = 0;

  std::string key;
  std::string val;

  for (size_t i = 0; i < sl; i++)
    {
      char &c = s[i];

      switch (c)
        {
        case '{':
          {
            /*
              Certain json objects enclose
              entire object definition between {}
            */
            if (!i)
              {
                continue;
              }

            if (reading_key)
              {
                assert (in_str && "Keys can only be strings.");
              }

            if (!in_str)
              gb++;
          }
          break;

        case '}':
          {
            if (reading_key)
              {
                assert (in_str && "Keys can only be strings.");
              }

            if (i == sl - 1)
              {
                if (!in_str && !gb)
                  {
                    util::trim_string (key);
                    key.erase (0, 1);
                    key.pop_back ();

                    try
                      {
                        r[key] = _parse_val (val);
                        // dbg ("Found key: " << key);
                        // dbg ("Found val: " << val);
                        // dbg ("Found r[key]: " << *r[key]);
                        // dbg ("Found r[key].get_type(): "
                        //      << int (r[key]->get_type ()));
                      }
                    catch (const std::exception &e)
                      {
                        std::cerr << e.what () << '\n';
                      }

                    key = "";
                    val = "";
                    reading_key = true;
                    reading_val = false;
                  }
              }

            if (!in_str)
              gb--;
          }
          break;

        /* single quotes not allowed */
        // case '\'':
        case '\"':
          {
            if (in_str)
              {
                if (s[i - 1] == '\\')
                  {
                    if (s[i - 2] == '\\')
                      in_str = false; /* \\" */
                  }
                else
                  in_str = false;
              }
            else
              {
                in_str = true;
              }
          }
          break;

        case '[':
          {
            if (reading_key)
              {
                assert (in_str && "Keys can only be strings.");
              }

            if (!in_str)
              gb++;
          }
          break;

        case ']':
          {
            if (reading_key)
              {
                assert (in_str && "Keys can only be strings.");
              }

            if (!in_str)
              gb--;
          }
          break;

        case ':':
          {
            if (!in_str && !gb)
              {
                reading_key = false;
                reading_val = true;
                continue;
              }
          }
          break;

        case ',':
          {
            if (!in_str && !gb)
              {
                util::trim_string (key);
                key.erase (0, 1);
                key.pop_back ();

                try
                  {
                    JsonObject *jo = _parse_val (val);
                    // std::cout << int (jo->get_type ()) << '\n';
                    r[key] = jo;
                    // dbg ("Found key: " << key);
                    // dbg ("Found val: " << val);
                    // dbg ("Found r[key]: " << *r[key]);
                    // dbg ("Found r[key].get_type(): "
                    //      << int (r[key]->get_type ()));
                  }
                catch (const std::exception &e)
                  {
                    std::cerr << e.what () << '\n';
                  }

                key = "";
                val = "";
                reading_key = true;
                reading_val = false;
                continue;
              }
          }
          break;

        default:
          break;
        }

      if (reading_key)
        key += c;

      if (reading_val)
        val += c;
    }

  if (!key.empty () && !val.empty ())
    {
      util::trim_string (key);
      key.erase (0, 1);
      key.pop_back ();

      try
        {
          r[key] = _parse_val (val);
        }
      catch (const std::exception &e)
        {
          std::cerr << e.what () << '\n';
        }
    }

  return r;
}

std::string
JsonContext::to_string ()
{
  std::stringstream ss;
  ss << "{";
  bool first = true;
  for (const auto &[key, value] : ctx)
    {
      if (!first)
        ss << ", ";
      first = false;
      // dbg ("key: " << key << "\tvalue << " << int (value->get_type ()));
      ss << "\"" << key << "\": " << *value;
    }
  ss << "}";
  return ss.str ();
}

int
JsonObject::to_integer () const
{
  if (is_integer ())
    {
      return as_integer ();
    }
  else if (is_float ())
    {
      return (int)as_float ();
    }
  else if (is_boolean ())
    {
      return (int)as_boolean ();
    }
  else if (is_null ())
    {
      return 0;
    }
  else
    throw std::invalid_argument ("Invalid conversion to int");

  return 0;
}

float
JsonObject::to_float () const
{
  if (is_integer ())
    {
      return (float)as_integer ();
    }
  else if (is_float ())
    {
      return as_float ();
    }
  else if (is_boolean ())
    {
      return as_boolean () ? 1.0f : 0.0f;
    }
  else if (is_null ())
    {
      return 0.0f;
    }
  else
    throw std::invalid_argument ("Invalid conversion to float of type "
                                 + std::to_string (int (get_type ())));

  return 0.0f;
}

std::string
JsonObject::to_string () const
{
  if (is_string ())
    return as_string ();
  else
    throw std::invalid_argument ("Invalid conversion to string of type: "
                                 + std::to_string (int (get_type ())));

  return "";
}

bool
JsonObject::to_boolean () const
{
  if (is_integer ())
    {
      return (bool)as_integer ();
    }
  else if (is_float ())
    {
      return as_float () != 0.0f;
    }
  else if (is_boolean ())
    {
      return as_boolean ();
    }
  else if (is_null ())
    {
      return false;
    }
  else
    throw std::invalid_argument ("Invalid conversion to boolean");

  return false;
}

bool
JsonContext::has_key (std::string k)
{
  return ctx.find (k) != ctx.end ();
}

} // namespace rs::json
