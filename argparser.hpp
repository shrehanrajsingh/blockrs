#if !defined(ARGPARSER_H)
#define ARGPARSER_H

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace rs
{

class Argument
{
private:
  std::string name;
  std::string short_flag;
  std::string long_flag;
  std::string description;
  bool required;
  bool is_flag;
  std::string default_value;
  std::string value;
  bool provided;
  std::function<bool (const std::string &)> validator;

public:
  Argument (const std::string &name, const std::string &short_flag,
            const std::string &long_flag, const std::string &description,
            bool required = false, bool is_flag = false,
            const std::string &default_value = "")
      : name (name), short_flag (short_flag), long_flag (long_flag),
        description (description), required (required), is_flag (is_flag),
        default_value (default_value), provided (false)
  {
    validator = [] (const std::string &) { return true; };
  }

  Argument &set_validator (std::function<bool (const std::string &)>);
  bool validate () const;
  void set_value (const std::string &);
  std::string get_name () const;
  std::string get_short_flag () const;
  std::string get_long_flag () const;
  std::string get_description () const;
  bool is_required () const;
  bool is_provided () const;
  bool get_is_flag () const;
  std::string get_value () const;

  template <typename T>
  T
  get_value_as () const
  {
    std::stringstream ss (get_value ());
    T result;
    ss >> result;
    return result;
  }
};

class ArgumentParser
{
public:
private:
  std::string program_name;
  std::string description;
  std::vector<Argument> arguments;
  std::map<std::string, size_t> short_flag_map;
  std::map<std::string, size_t> long_flag_map;
  std::map<std::string, size_t> name_map;

public:
  ArgumentParser (const std::string &program_name = "",
                  const std::string &description = "")
      : program_name (program_name), description (description)
  {
  }

  Argument &add_argument (const std::string &name,
                          const std::string &short_flag,
                          const std::string &long_flag,
                          const std::string &description,
                          bool required = false, bool is_flag = false,
                          const std::string &default_value = "");

  bool parse (int, char *[]);
  bool has (const std::string &) const;
  std::string get (const std::string &) const;

  template <typename T>
  T
  get (const std::string &name) const
  {
    auto it = name_map.find (name);
    if (it == name_map.end ())
      {
        throw std::invalid_argument ("Argument not found: " + name);
      }
    return arguments[it->second].get_value_as<T> ();
  }

  void print_help (std::ostream &os = std::cout) const;

  inline std::map<std::string, size_t> &
  get_short_flag_map ()
  {
    return short_flag_map;
  }

  inline std::map<std::string, size_t> &
  get_long_flag_map ()
  {
    return long_flag_map;
  }

  inline std::map<std::string, size_t> &
  get_name_map ()
  {
    return name_map;
  }
};
} // namespace rs

#endif // ARGPARSER_H
