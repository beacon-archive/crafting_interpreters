#pragma

#include <variant>
#include <string>

namespace beacon_lox
{

using LoxDouble = double;
using LoxString = std::string;
using LoxNil = std::nullptr_t;
using LoxBoolean = bool;

using LoxObject =
    std::variant<std::monostate, std::string, double, bool, std::nullptr_t>;

std::string
to_string(LoxObject const& obj);

bool
is_true(LoxObject const& obj);

} // namespace beacon_lox