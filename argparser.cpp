#include "argparser.hpp"

namespace rs
{
Argument &
Argument::set_validator (
    std::function<bool (const std::string &)> validator_fn)
{
  validator = validator_fn;
  return *this;
}

bool
Argument::validate () const
{
  if (is_flag)
    return true;
  if (!provided && required)
    return false;
  if (!provided)
    return true;
  return validator (value);
}

void
Argument::set_value (const std::string &val)
{
  value = val;
  provided = true;
}

std::string
Argument::get_name () const
{
  return name;
}
std::string
Argument::get_short_flag () const
{
  return short_flag;
}
std::string
Argument::get_long_flag () const
{
  return long_flag;
}
std::string
Argument::get_description () const
{
  return description;
}
bool
Argument::is_required () const
{
  return required;
}
bool
Argument::is_provided () const
{
  return provided;
}
bool
Argument::get_is_flag () const
{
  return is_flag;
}

std::string
Argument::get_value () const
{
  if (provided)
    return value;
  return default_value;
}

Argument &
ArgumentParser::add_argument (const std::string &name,
                              const std::string &short_flag,
                              const std::string &long_flag,
                              const std::string &description, bool required,
                              bool is_flag, const std::string &default_value)
{

  if (!short_flag.empty ()
      && short_flag_map.find (short_flag) != short_flag_map.end ())
    {
      throw std::invalid_argument ("Short flag already exists: " + short_flag);
    }

  if (!long_flag.empty ()
      && long_flag_map.find (long_flag) != long_flag_map.end ())
    {
      throw std::invalid_argument ("Long flag already exists: " + long_flag);
    }

  if (name_map.find (name) != name_map.end ())
    {
      throw std::invalid_argument ("Argument name already exists: " + name);
    }

  arguments.emplace_back (name, short_flag, long_flag, description, required,
                          is_flag, default_value);

  if (!short_flag.empty ())
    {
      short_flag_map[short_flag] = arguments.size () - 1;
    }

  if (!long_flag.empty ())
    {
      long_flag_map[long_flag] = arguments.size () - 1;
    }

  name_map[name] = arguments.size () - 1;

  return arguments.back ();
}

bool
ArgumentParser::parse (int argc, char *argv[])
{
  if (argc > 0)
    {
      program_name = argv[0];
    }

  for (int i = 1; i < argc; ++i)
    {
      std::string arg = argv[i];

      if (arg.size () >= 2 && arg[0] == '-')
        {
          if (arg.size () >= 3 && arg[1] == '-')
            {
              std::string flag = arg.substr (2);
              auto it = long_flag_map.find (flag);

              if (it == long_flag_map.end ())
                {
                  std::cerr << "Unknown argument: " << arg << std::endl;
                  return false;
                }

              Argument &argument = arguments[it->second];

              if (argument.get_is_flag ())
                {
                  argument.set_value ("true");
                }
              else
                {
                  if (i + 1 >= argc)
                    {
                      std::cerr << "Expected value for argument: " << arg
                                << std::endl;
                      return false;
                    }

                  argument.set_value (argv[++i]);
                }
            }
          else
            {

              std::string flag = arg.substr (1);
              auto it = short_flag_map.find (flag);

              if (it == short_flag_map.end ())
                {
                  std::cerr << "Unknown argument: " << arg << std::endl;
                  return false;
                }

              Argument &argument = arguments[it->second];

              if (argument.get_is_flag ())
                {
                  argument.set_value ("true");
                }
              else
                {
                  if (i + 1 >= argc)
                    {
                      std::cerr << "Expected value for argument: " << arg
                                << std::endl;
                      return false;
                    }

                  argument.set_value (argv[++i]);
                }
            }
        }
      else
        {
          std::cerr << "Unexpected argument: " << arg << std::endl;
          return false;
        }
    }

  for (const auto &argument : arguments)
    {
      if (!argument.validate ())
        {
          if (argument.is_required () && !argument.is_provided ())
            {
              std::cerr << "Required argument missing: ";
              if (!argument.get_short_flag ().empty ())
                std::cerr << "-" << argument.get_short_flag ();
              else
                std::cerr << "--" << argument.get_long_flag ();
              std::cerr << std::endl;
            }
          else
            {
              std::cerr << "Invalid value for argument" << std::endl;
            }
          return false;
        }
    }

  return true;
}

bool
ArgumentParser::has (const std::string &name) const
{
  auto it = name_map.find (name);
  if (it == name_map.end ())
    return false;
  return arguments[it->second].is_provided ();
}

std::string
ArgumentParser::get (const std::string &name) const
{
  auto it = name_map.find (name);
  if (it == name_map.end ())
    {
      throw std::invalid_argument ("Argument not found: " + name);
    }
  return arguments[it->second].get_value ();
}

void
ArgumentParser::print_help (std::ostream &os) const
{
  os << "Usage: " << program_name << " [options]" << std::endl;

  if (!description.empty ())
    {
      os << description << std::endl;
    }

  os << "Options:" << std::endl;

  for (const auto &argument : arguments)
    {
      os << "  ";

      if (!argument.get_short_flag ().empty ())
        {
          os << "-" << argument.get_short_flag ();

          if (!argument.get_long_flag ().empty ())
            {
              os << ", ";
            }
        }

      if (!argument.get_long_flag ().empty ())
        {
          os << "--" << argument.get_long_flag ();
        }

      if (!argument.get_is_flag ())
        {
          os << " <value>";
        }

      os << "\t" << argument.get_description ();

      if (argument.is_required ())
        {
          os << " (required)";
        }
      else if (!argument.get_is_flag () && !argument.get_value ().empty ())
        {
          os << " (default: " << argument.get_value () << ")";
        }

      os << std::endl;
    }
}
} // namespace rs
