#if !defined(JSON_H)
#define JSON_H

#include "../header.hpp"

namespace rs::json
{
enum class JsonType
{
  Integer = 0,
  Float = 1,
  String = 2,
  Object = 3,
  Boolean = 4,
  Array = 5,
  Null = 6,
  NoType
};

class JsonObject
{
  JsonType type;

  union
  {
    /* primitives */
    int jint;
    float jfloat;
    std::string jstr;
    JsonObject *jobj;
    bool jbool;
    std::vector<JsonObject *> jarray;
    /* no value for null, just type is enough */
  };

public:
  JsonObject () : type (JsonType::NoType) {}
  JsonObject (JsonType _Type) : type (_Type) {}

  JsonObject (const char *_Val) : type (JsonType::String), jstr (_Val) {}
  JsonObject (std::string _Val) : type (JsonType::String), jstr (_Val) {}
  JsonObject (int _Val) : type (JsonType::Integer), jint (_Val) {}
  JsonObject (bool _Val) : type (JsonType::Boolean), jbool (_Val) {}
  JsonObject (float _Val) : type (JsonType::Float), jfloat (_Val) {}
  JsonObject (JsonObject *_Val) : type (JsonType::Object), jobj (_Val) {}
  JsonObject (std::vector<JsonObject *> _Val)
      : type (JsonType::Array), jarray (_Val)
  {
  }

  inline JsonType &
  get_type ()
  {
    return type;
  }

  JsonObject &operator= (const JsonObject &);
  friend std::ostream &operator<< (std::ostream &, JsonObject &);

  ~JsonObject () {}
};

class JsonContext
{
private:
  std::map<std::string, JsonObject *> ctx;

public:
  JsonContext ();
  JsonContext (std::initializer_list<std::pair<std::string, JsonObject>>);

  inline std::map<std::string, JsonObject *> &
  get_ctx ()
  {
    return ctx;
  }

  JsonObject &operator[] (std::string);

  ~JsonContext ();
};

typedef JsonContext json_t;
} // namespace rs::json

#endif // JSON_H
