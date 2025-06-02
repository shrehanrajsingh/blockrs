#if !defined(JSON_H)
#define JSON_H

#include "../header.hpp"

#define J(X) (*(X))

#if !defined(JSON_RAW)
#define JSON_RAW(...) #__VA_ARGS__
#endif

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

class JsonContext;
class JsonObject
{
  JsonType type;

public:
  union
  {
    /* primitives */
    int jint;
    float jfloat;
    std::string jstr;
    JsonContext *jobj;
    bool jbool;
    std::vector<JsonObject *> jarray;
    /* no value for null, just type is enough */
  };
  JsonObject () : type (JsonType::NoType) {}
  JsonObject (JsonType _Type) : type (_Type) {}

  JsonObject (const char *_Val) : type (JsonType::String), jstr (_Val) {}
  JsonObject (std::string _Val) : type (JsonType::String), jstr (_Val) {}
  JsonObject (int _Val) : type (JsonType::Integer), jint (_Val) {}
  JsonObject (bool _Val) : type (JsonType::Boolean), jbool (_Val) {}
  JsonObject (float _Val) : type (JsonType::Float), jfloat (_Val) {}
  JsonObject (JsonContext *_Val) : type (JsonType::Object), jobj (_Val) {}
  JsonObject (std::vector<JsonObject *> _Val)
      : type (JsonType::Array), jarray (_Val)
  {
  }

  inline JsonType &
  get_type ()
  {
    return type;
  }

  inline const JsonType &
  get_type () const
  {
    return type;
  }

  JsonObject &operator= (const JsonObject &);
  friend std::ostream &operator<< (std::ostream &, JsonObject &);

  bool
  is_integer () const
  {
    return type == JsonType::Integer;
  }
  bool
  is_float () const
  {
    return type == JsonType::Float;
  }
  bool
  is_string () const
  {
    return type == JsonType::String;
  }
  bool
  is_object () const
  {
    return type == JsonType::Object;
  }
  bool
  is_boolean () const
  {
    return type == JsonType::Boolean;
  }
  bool
  is_array () const
  {
    return type == JsonType::Array;
  }
  bool
  is_null () const
  {
    return type == JsonType::Null;
  }

  int
  as_integer () const
  {
    if (type != JsonType::Integer)
      throw std::runtime_error ("JSON value is not an integer. type: "
                                + std::to_string (int (get_type ())));
    return jint;
  }

  float
  as_float () const
  {
    if (type != JsonType::Float)
      throw std::runtime_error ("JSON value is not a float. type: "
                                + std::to_string (int (get_type ())));
    return jfloat;
  }

  const std::string &
  as_string () const
  {
    if (type != JsonType::String)
      throw std::runtime_error ("JSON value is not a string. type: "
                                + std::to_string (int (get_type ())));
    return jstr;
  }

  JsonContext *
  as_object () const
  {
    if (type != JsonType::Object)
      throw std::runtime_error ("JSON value is not an object. type: "
                                + std::to_string (int (get_type ())));
    return jobj;
  }

  bool
  as_boolean () const
  {
    if (type != JsonType::Boolean)
      throw std::runtime_error ("JSON value is not a boolean. type: "
                                + std::to_string (int (get_type ())));
    return jbool;
  }

  const std::vector<JsonObject *> &
  as_array () const
  {
    if (type != JsonType::Array)
      throw std::runtime_error ("JSON value is not an array. type: "
                                + std::to_string (int (get_type ())));
    return jarray;
  }

  int to_integer () const;
  float to_float () const;
  std::string to_string () const;
  bool to_boolean () const;

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

  static JsonContext from_string (std::string);
  JsonObject *&operator[] (std::string);
  std::string to_string ();

  bool has_key (std::string);

  friend std::ostream &operator<< (std::ostream &, JsonContext &);

  ~JsonContext ();
};

typedef JsonContext json_t;
} // namespace rs::json

#endif // JSON_H
