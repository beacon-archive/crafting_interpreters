#pragma once

#include "frontend/token.hh"
#include "interpreter/lox_object.hh"


#include <memory>
#include <utility>
#include <unordered_map>

namespace beacon_lox
{
class Environment;
using EnvironmentPrt = std::shared_ptr<Environment>;

class Environment : public std::enable_shared_from_this<Environment>
{
public:
  Environment() = default;

  Environment(EnvironmentPrt environment)
    : enclosing_(std::move(environment))
  {}

  // 1. 获取变量
  LoxObject
  // get(std::string_view name) const;
  get(Token const& name);

  // 2. 创建变量, lox 支持重定义.
  void
  define(std::string_view name, LoxObject lox_obj = LoxNil{});

  // 3. 赋值变量
  void
  assign(Token const& name, LoxObject const& value);

  // 下面是自己没有想到的...
  EnvironmentPrt
  get_enclosing() const
  {
    return enclosing_;
  }



private:
  EnvironmentPrt enclosing_;
  std::unordered_map<std::string_view, LoxObject> values_;
};
} // namespace beacon_lox