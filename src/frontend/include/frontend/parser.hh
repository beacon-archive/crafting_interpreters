#pragma once
#include <vector>

#include "error.hh"
#include "ast.hh"
#include "token.hh"


namespace beacon_lox
{
class Parser
{
public:
  Parser(std::vector<Token> token)
    : tokens_(std::move(token))
  // , cur_iter_(tokens_.begin())
  // , end_iter_(tokens_.end())
  {}

  auto
  parse() -> Expr
  {
    return expression();
  }

private:
  auto
  expression() -> Expr
  {
    return equality();
  }

  auto
  equality() -> Expr
  {
    Expr expr = comparsion();
    while(match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
    {
      // Token operator = p
      auto op = previos();
      Expr right = comparsion();
      expr = std::make_unique<BinaryExpr>(std::move(expr),
                                          op,
                                          static_cast<BinaryOp>(op.get_type()),
                                          std::move(right));
    }

    return expr;
  }

  auto
  comparsion() -> Expr
  {
    Expr exp = term();
    while(match(TokenType::GREATER,
                TokenType::GREATER_EQUAL,
                TokenType::LESS,
                TokenType::LESS_EQUAL))
    {
      auto op = previos();
      Expr right = term();
      exp = std::make_unique<BinaryExpr>(std::move(exp),
                                         op,
                                         static_cast<BinaryOp>(op.get_type()),
                                         std::move(right));
    }
    return exp;
  }

  auto
  term() -> Expr
  {
    Expr exp = factor();
    while(match(TokenType::PLUS, TokenType::MINUS))
    {
      auto op = previos();
      Expr right = factor();
      exp = std::make_unique<BinaryExpr>(std::move(exp),
                                         op,
                                         static_cast<BinaryOp>(op.get_type()),
                                         std::move(right));
    }
    return exp;
  }

  auto
  factor() -> Expr
  {
    Expr exp = unary();
    while(match(TokenType::STAR, TokenType::SLASH))
    {
      auto op = previos();
      Expr right = unary();
      exp = std::make_unique<BinaryExpr>(std::move(exp),
                                         op,
                                         static_cast<BinaryOp>(op.get_type()),
                                         std::move(right));
    }
    return exp;
  }

  auto
  unary() -> Expr
  {
    if(match(TokenType::BANS, TokenType::MINUS))
    {
      auto op = previos();
      auto expr = primary();
      Expr exp =
          std::make_unique<UnaryExpr>(std::move(expr),
                                      op,
                                      static_cast<UnaryOp>(op.get_type()));
      return exp;
    }

    return primary();
  }

  auto
  primary() -> Expr
  {
    if(match(TokenType::FALSE))
    {
      return std::make_unique<LiteralExpr>(false);
    }
    if(match(TokenType::TRUE))
    {
      return std::make_unique<LiteralExpr>(true);
    }
    if(match(TokenType::NIL))
    {
      return std::make_unique<LiteralExpr>(nullptr);
    }
    if(match(TokenType::NUMBER, TokenType::STRING))
    {
      auto token = previos();
      return std::make_unique<LiteralExpr>(token.get_literal());
    }

    if(match(TokenType::LEFT_PAREN))
    {
      Expr exp = expression();
      consume(TokenType::RIGHT_PAREN, "( not match!");
      return std::make_unique<GroupingExpr>(std::move(exp));
    }

    throw std::runtime_error("Expect expression.");
  }

  auto
  consume(const TokenType &type, std::string_view sv) -> void
  {
    if(check(type))
    {
      advance();
      return;
    }
    // std::string str;

    // str += "cur type:";
    // str += std::to_string(static_cast<int>(tokens_[cur_].get_type()));
    // str += sv;
    // throw std::runtime_error(str);
    g_error.error(error(peek(), sv));
  }


  auto
  error(Token token, std::string_view sv) -> Error::RuntimeError
  {
    throw Error::RuntimeError(token, std::string(sv));
  }

  class ParseError : public std::runtime_error
  {
  public:
    ParseError(const std::string &message = "Parse error occurred")
      : std::runtime_error(message)
    {}
  };

  auto
  synchronize() -> void
  {
    advance();
    while(!is_at_end())
    {
      if(previos().get_type() == TokenType::SEMICOLON)
      {
        return;
      }

      switch(previos().get_type())
      {
        case CLASS:
        case FUN:
        case VAR:
        case FOR:
        case IF:
        case PRINT:
        case WHILE:
        case RETURN:
          return;
        default:
          break;
      }
      advance();
    }
  }

  // 关于 match 自己的意图和作者的存在区别
  // 作者书中的意图是当发现匹配时,就会消耗掉这个token
  // [[nodiscard]] auto
  // match(const TokenType &type) const -> bool
  // {
  //   if(is_at_end())
  //   {
  //     return false;
  //   }

  //   return type == tokens_[cur_].get_type();
  // }
  //* 可以合并到一个模板参数中
  // [[nodiscard]] auto
  // match(const TokenType &type) -> bool
  // {
  //   if(check(type))
  //   {
  //     advance();
  //     return true;
  //   }
  //   return false;
  // }

  // [[nodiscard]] auto
  // check(const TokenType &type) const -> bool
  // {
  //   if(is_at_end())
  //   {
  //     return false;
  //   }

  //   return type == tokens_[cur_].get_type();
  // }
  template <typename... Args>
  [[nodiscard]] auto
  check(const Args &...args) const -> bool
  {
    if(is_at_end())
    {
      return false;
    }

    // 这种空参数包是自己没有想到的, gpt 进行了补充
    if constexpr(sizeof...(args) == 0)
    {
      return false; // 空参数包
    }
    return ((tokens_[cur_].get_type() == args) || ...);
  }

  template <typename... Args>
  [[nodiscard]] auto
  match(Args... args) -> bool
  {
    // 添加静态断言, 确保传入的参数类型符合预期
    static_assert((std::is_same_v<std::decay_t<Args>, TokenType> && ...),
                  "All arguments must be of type TokenType");

    if(check(args...))
    {
      advance();
      return true;
    }

    return false;
  }

  [[nodiscard]] auto
  peek() -> const Token &
  {
    // 感觉这里应该还是要加一个处理的
    //   if(cur_iter_ == end_iter_)
    //   {
    //     return
    //   }
    return tokens_[cur_];
  }

  [[nodiscard]] auto
  previos() -> const Token &
  {
    return tokens_[cur_ - 1];
  }

  [[nodiscard]] auto
  is_at_end() const -> bool
  {
    return tokens_[cur_].get_type() == TokenType::LOX_EOF;
  }

  // 昨天自己有个疑问, 如果超过最后一个字符要怎么处理,这里显示了结果
  // 如果已经到了最后了, 不移动 迭代器 就可以了
  void
  advance()
  {
    if(is_at_end())
    {
      return;
    }
    ++cur_;
  }

  std::vector<Token> tokens_;
  // std::vector<Token>::iterator cur_iter_;
  int cur_{0};
  // 预期: 每个 token 序列的最后一个都是 EOF!
  // 所以这里没有必要单独存储一个 end 了
  // std::vector<Token>::iterator end_iter_;
};
} // namespace beacon_lox