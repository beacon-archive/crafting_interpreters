#pragma once

#include "token.hh"
#include "utils.hh"
#include <any>
#include <variant>
#include <memory>

namespace beacon_lox
{

enum class UnaryOp
{
  BANS = static_cast<int>(TokenType::BANS),
  MINUS = static_cast<int>(TokenType::MINUS),
};

enum class BinaryOp
{
  PLUS = static_cast<int>(TokenType::PLUS),
  MINUS = static_cast<int>(TokenType::MINUS),
  SLASH = static_cast<int>(TokenType::SLASH),
  STAR = static_cast<int>(TokenType::STAR),
  BANG_EQUAL = static_cast<int>(TokenType::BANG_EQUAL),
  EQUAL_EQUAL = static_cast<int>(TokenType::EQUAL_EQUAL),
  GREATER = static_cast<int>(TokenType::GREATER),
  GREATER_EQUAL = static_cast<int>(TokenType::GREATER_EQUAL),
  LESS = static_cast<int>(TokenType::LESS),
  LESS_EQUAL = static_cast<int>(TokenType::LESS_EQUAL),
};


} // namespace beacon_lox


namespace std
{
template <>
class formatter<beacon_lox::UnaryOp>
{
public:
  constexpr auto
  parse(format_parse_context &ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(const beacon_lox::UnaryOp &op, FormatContext &ctx) const
  {
    switch(op)
    {
      case beacon_lox::UnaryOp::BANS:
        return std::format_to(ctx.out(), "BANS");
      case beacon_lox::UnaryOp::MINUS:
        return std::format_to(ctx.out(), "MINUS");
      default:
        return std::format_to(ctx.out(), "UNKOWN");
    }
  }
};

template <>
class formatter<beacon_lox::BinaryOp>
{
public:
  constexpr auto
  parse(format_parse_context &ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(const beacon_lox::BinaryOp &op, FormatContext &ctx) const
  {
    switch(op)
    {
      case beacon_lox::BinaryOp::PLUS:
        return std::format_to(ctx.out(), "PLUS");
      case beacon_lox::BinaryOp::MINUS:
        return std::format_to(ctx.out(), "MINUS");
      case beacon_lox::BinaryOp::SLASH:
        return std::format_to(ctx.out(), "SLASH");
      case beacon_lox::BinaryOp::STAR:
        return std::format_to(ctx.out(), "STAR");
      case beacon_lox::BinaryOp::BANG_EQUAL:
        return std::format_to(ctx.out(), "BANG_EQUAL");
      case beacon_lox::BinaryOp::EQUAL_EQUAL:
        return std::format_to(ctx.out(), "EQUAL_EQUAL");
      case beacon_lox::BinaryOp::LESS_EQUAL:
        return std::format_to(ctx.out(), "LESS_EQUAL");
      case beacon_lox::BinaryOp::GREATER_EQUAL:
        return std::format_to(ctx.out(), "GREATER_EQUAL");
      case beacon_lox::BinaryOp::LESS:
        return std::format_to(ctx.out(), "LESS");
      case beacon_lox::BinaryOp::GREATER:
        return std::format_to(ctx.out(), "GREATER");
      default:
        return std::format_to(ctx.out(), "UNKNOWN");
    }
  }
};

} // namespace std



namespace beacon_lox
{
class LiteralExpr;
class UnaryExpr;
class BinaryExpr;
class GroupingExpr;

using LiteralExprPtr = std::unique_ptr<LiteralExpr>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using GroupingExprPtr = std::unique_ptr<GroupingExpr>;
using Expr =
    std::variant<LiteralExprPtr, UnaryExprPtr, BinaryExprPtr, GroupingExprPtr>;

class Visitor
{
public:
  virtual ~Visitor() = default;
  virtual std::any
  literal_expr_visitor(LiteralExpr *) = 0;
  virtual std::any
  unary_expr_visitor(UnaryExpr *) = 0;
  virtual std::any
  binary_expr_visitor(BinaryExpr *) = 0;
  virtual std::any
  grouping_expr_visitor(GroupingExpr *) = 0;
};

class ExprBase : private Uncopyabble
{
public:
  virtual ~ExprBase() = default;
  virtual std::any
  accept(Visitor *visitor) = 0;
};

class LiteralExpr : private ExprBase
{
public:
  Literal literal;
  explicit LiteralExpr(Literal _literal)
    : literal(_literal)
  {}
  std::any
  accept(Visitor *visitor) override
  {
    return visitor->literal_expr_visitor(this);
  }
};

class BinaryExpr : private ExprBase
{
public:
  Expr left;
  Token token;
  BinaryOp op;
  Expr right;
  explicit BinaryExpr(Expr _left, Token _token, BinaryOp _op, Expr _right)
    : left(std::move(_left))
    , token(_token)
    , op(_op)
    , right(std::move(_right))
  {}
  std::any
  accept(Visitor *visitor) override
  {
    return visitor->binary_expr_visitor(this);
  }
};

class UnaryExpr : private ExprBase
{
public:
  Expr expr;
  Token token;
  UnaryOp op;

  explicit UnaryExpr(Expr _expr, Token _token, UnaryOp _op)
    : expr(std::move(_expr))
    , token(_token)
    , op(_op)
  {}
  std::any
  accept(Visitor *visitor) override
  {
    return visitor->unary_expr_visitor(this);
  }
};

class GroupingExpr : private ExprBase
{
public:
  explicit GroupingExpr(Expr _expr)
    : expr(std::move(_expr))
  {}

  Expr expr;

  std::any
  accept(Visitor *visitor) override
  {
    return visitor->grouping_expr_visitor(this);
  }
};

class ExprVisitor : public Visitor
{
public:
  std::any
  literal_expr_visitor(LiteralExpr *literal) override
  {
    return std::format("({})", literal->literal);
  }
  // std::any
  // unary_expr_visitor(UnaryExpr *unary) override {
  //! 这里的问题是,这里的表达式是一个 variant, 不是一个 Expr, 不能直接调用其 accept 方法
  //! 这里的替代方案是可以使用一个 函数对象来.
  //! 忘记了 virant 的 函数对象怎么使用了
  //! 这里自己的问题, 还是想着根据不同的表达式类别去掉各自的函数....
  // return std::format("({] {} {}})",
  //                    unary->op,
  //                    unary->token.get_lexeme(),
  //                    std::visit([](const auto &value) -> std::string
  //                               { return std::format("{}", value); },
  //                               unary->expr));
  std::any
  unary_expr_visitor(UnaryExpr *unary) override
  {
    return std::format(
        "({} {} {})",
        unary->op,
        unary->token.get_lexeme(),
        std::visit([this](const auto &value) -> std::string
                   { return std::any_cast<std::string>(value->accept(this)); },
                   unary->expr));
  }
  std::any
  binary_expr_visitor(BinaryExpr *binary) override
  {
    return std::format(
        "({} {} {} {})",
        binary->op,
        binary->token.get_lexeme(),
        std::visit([this](const auto &value) -> std::string
                   { return std::any_cast<std::string>(value->accept(this)); },
                   binary->left),
        std::visit([this](const auto &value) -> std::string
                   { return std::any_cast<std::string>(value->accept(this)); },
                   binary->right));
  }
  std::any
  grouping_expr_visitor(GroupingExpr *grouping) override
  {
    return std::format(
        "(grouping {})",
        std::visit([this](const auto &value) -> std::string
                   { return std::any_cast<std::string>(value->accept(this)); },
                   grouping->expr));
  }
};

} // namespace beacon_lox
