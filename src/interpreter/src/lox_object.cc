#include "interpreter/lox_object.hh"

#include <sstream>


namespace beacon_lox
{

std::string
to_string(LoxObject const& obj)
{
  return std::visit(
      [](auto&& value) -> std::string
      {
        using T = std::decay_t<decltype(value)>;

        if constexpr(std::is_same_v<T, std::monostate>)
        {
          return "nil";
        }
        if constexpr(std::is_same_v<T, std::nullptr_t>)
        {
          return "nil";
        }
        if constexpr(std::is_same_v<T, bool>)
        {
          return value ? "true" : "false";
        }
        if constexpr(std::is_same_v<T, double>)
        {
          // 避免小数点后显示 .0
          std::ostringstream oss;
          oss << value;
          return oss.str();
        }
        if constexpr(std::is_same_v<T, std::string>)
        {
          return value;
        }
        throw std::runtime_error("unkown type for LoxObject");
      },
      obj);
}

bool
is_true(LoxObject const& obj)
{
  if(std::holds_alternative<LoxNil>(obj))
  {
    return false;
  }
  if(std::holds_alternative<LoxBoolean>(obj))
  {
    return std::get<LoxBoolean>(obj);
  }
  return true;
}
} // namespace beacon_lox