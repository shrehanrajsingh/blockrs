#include "json.hpp"

namespace rs::json
{
JsonObject &
JsonObject::operator= (const JsonObject &rhs)
{
  if (this != &rhs)
    {
      type = rhs.type;
      switch (rhs.type)
        {
        case JsonType::Array:
          {
            jarray = rhs.jarray;
          }
          break;
        case JsonType::Boolean:
          {
            jbool = rhs.jbool;
          }
          break;
        case JsonType::Float:
          {
            jfloat = rhs.jfloat;
          }
          break;
        case JsonType::Integer:
          {
            jint = rhs.jint;
          }
          break;
        case JsonType::NoType:
          {
          }
          break;
        case JsonType::Null:
          {
          }
          break;
        case JsonType::Object:
          {
            /* keep it this way to prevent recursion */
            jobj = rhs.jobj;
          }
          break;
        case JsonType::String:
          {
            jstr = rhs.jstr;
          }
          break;
        default:
          {
          }
          break;
        }
    }
  return *this;
}

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

JsonObject &
JsonContext::operator[] (std::string s)
{
  if (ctx.find (s) == ctx.end ())
    ctx[s] = new JsonObject;
  return *ctx[s];
}

std::ostream &
operator<< (std::ostream &out, JsonObject &lhs)
{
  switch (lhs.type)
    {
    case JsonType::Array:
      {
        out << "[";
        bool first = true;
        for (auto &item : lhs.jarray)
          {
            if (!first)
              out << ",";
            first = false;
            out << item;
          }
        out << "]";
      }
      break;
    case JsonType::Boolean:
      {
        out << (lhs.jbool ? "true" : "false");
      }
      break;
    case JsonType::Float:
      {
        out << lhs.jfloat;
      }
      break;
    case JsonType::Integer:
      {
        out << lhs.jint;
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
        out << *lhs.jobj;
        out << "}";
      }
      break;
    case JsonType::String:
      {
        out << "\"";
        for (char c : lhs.jstr)
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
} // namespace rs::json
