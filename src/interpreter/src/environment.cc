#include <utility>

#include "frontend/error.hh"

#include "interpreter/environment.hh"


namespace beacon_lox
{
void
Environment::define(std::string_view name, LoxObject lox_obj)
{
  // 1. 判断是否已经存在,如果存在则抛出异常
  // ? 但是 lox 好像允许重定义变量

  // 2. 创建新的变量
  values_[name] = std::move(lox_obj);
}

LoxObject
Environment::get(Token const& name)
{
  // 1. 查找 vlaue 中是否存在
  // 这里还是想着用 find 和 迭代器,但是有更好的方法!
  if(values_.contains(name.get_lexeme()))
  {
    return values_[name.get_lexeme()];
  }
  // 2. 遍历 enclosing
  // EnvironmentPrt enclosing = enclosing_;
  // while(enclosing != nullptr)
  // {
  //   if(enclosing->values_.contains(name.get_lexeme()))
  //   {
  //     return enclosing->values_[name.get_lexeme()];
  //   }
  //   enclosing = enclosing->enclosing_;
  // }
  if(enclosing_ != nullptr)
  {
    return enclosing_->get(name);
  }

  // 3. 抛出异常
  throw RuntimeError(
      name,
      std::format("Undefined variable: '{}'.", name.get_lexeme()));
}

void
Environment::assign(Token const& name, LoxObject const& value)
{
  if(values_.contains(name.get_lexeme()))
  {
    values_[name.get_lexeme()] = value;
    return;
  }

  // 这里的想法还是和 get 一样,一直去遍历...看下别人怎么写的
  // * 醉了...别人也是递归调用的,自己没有想到啊
  // 关于递归调用的内容,自己还是要好好思考一下
  if(enclosing_ != nullptr)
  {
    enclosing_->assign(name, value);
  }

  throw RuntimeError(
      name,
      std::format("Undefined variable: '{}'.", name.get_lexeme()));
}

} // namespace beacon_lox