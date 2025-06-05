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
  using ArrayType = std::vector<JsonObject *>;
  using ObjectType = JsonContext *;

  // union
  // {
  //   /* primitives */
  //   int jint;
  //   float jfloat;
  //   std::string jstr;
  //   JsonContext *jobj;
  //   bool jbool;
  //   std::vector<JsonObject *> jarray;
  //   /* no value for null, just type is enough */
  // };

  std::variant<std::monostate, int, float, std::string, ObjectType, bool,
               ArrayType>
      value;

  JsonObject () : type (JsonType::NoType), value (std::monostate{}) {}
  JsonObject (std::nullptr_t) : type (JsonType::Null), value (std::monostate{})
  {
  }
  JsonObject (const char *_Val)
      : type (JsonType::String), value (std::string (_Val))
  {
  }
  JsonObject (char *_Val) : type (JsonType::String), value (std::string (_Val))
  {
  }
  JsonObject (std::string _Val) : type (JsonType::String), value (_Val) {}
  JsonObject (int _Val) : type (JsonType::Integer), value (_Val) {}
  JsonObject (bool _Val) : type (JsonType::Boolean), value (_Val) {}
  JsonObject (float _Val) : type (JsonType::Float), value (_Val) {}
  JsonObject (JsonContext *_Val) : type (JsonType::Object), value (_Val) {}
  JsonObject (std::vector<JsonObject *> _Val)
      : type (JsonType::Array), value (_Val)
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

  JsonObject (const JsonObject &) = default;
  JsonObject (JsonObject &&) noexcept = default;
  JsonObject &operator= (const JsonObject &) = default;
  JsonObject &operator= (JsonObject &&) noexcept = default;

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
    return std::get<int> (value);
  }

  float
  as_float () const
  {
    if (type != JsonType::Float)
      throw std::runtime_error ("JSON value is not a float. type: "
                                + std::to_string (int (get_type ())));
    return std::get<float> (value);
  }

  const std::string &
  as_string () const
  {
    if (type != JsonType::String)
      throw std::runtime_error ("JSON value is not a string. type: "
                                + std::to_string (int (get_type ())));
    return std::get<std::string> (value);
  }

  JsonContext *
  as_object () const
  {
    if (type != JsonType::Object)
      throw std::runtime_error ("JSON value is not an object. type: "
                                + std::to_string (int (get_type ())));
    return std::get<JsonContext *> (value);
  }

  bool
  as_boolean () const
  {
    if (type != JsonType::Boolean)
      throw std::runtime_error ("JSON value is not a boolean. type: "
                                + std::to_string (int (get_type ())));
    return std::get<bool> (value);
  }

  const std::vector<JsonObject *> &
  as_array () const
  {
    if (type != JsonType::Array)
      throw std::runtime_error ("JSON value is not an array. type: "
                                + std::to_string (int (get_type ())));
    return std::get<ArrayType> (value);
  }

  int to_integer () const;
  float to_float () const;
  std::string to_string () const;
  bool to_boolean () const;

  ~JsonObject () = default;
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
