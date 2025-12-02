#pragma once

#include <vector>
#include <string>
#include <format>

#include "token.hh"


namespace beacon_lox
{
enum class LoxStatus : int8_t
{
  OK,
  ERROR
};


class Error
{
public:
  struct RuntimeError : public std::runtime_error
  {
    Token _token;
    explicit RuntimeError(Token const& token, std::string const& msg)
      : std::runtime_error(msg)
      , _token(token)
    {}
  };
  void
  error(long unsigned int const line, std::string_view const msg)
  {
    report(line, "", msg);
  }
  void
  error(Token const& token, std::string_view const msg)
  {
    if(token.get_type() == TokenType::LOX_EOF)
    {
      report(token.get_line(), " at end", msg);
      return;
    }

    report(token.get_line(),
           " at '" + std::string(token.get_lexeme()) + "'",
           msg);
  }

  void
  error(RuntimeError const& runtime_error)
  {
    error(runtime_error._token, runtime_error.what());
  }


private:
  void
  report(long unsigned int const line,
         std::string_view const where,
         std::string_view const msg)
  {
    errs_.emplace_back(
        std::format("[line {}] Error{}: {}\n", line, where, msg));
  }
  std::vector<std::string> errs_;
  LoxStatus status_{LoxStatus::OK};
};

// 全局错误实例（可选，方便使用宏）
extern Error g_error;


struct RuntimeError final : public std::runtime_error
{
  Token token;
  explicit RuntimeError(Token const& _token, std::string const& message)
    : std::runtime_error(message)
    , token(_token)
  {}
};


} // namespace beacon_lox