//////////////////////////
/// @file ast.hh
/// @brief 代码的表示形式(Expression): 它应该易于解析器生成，也易于解释器使用
///        从直观上看，我们的代码的一种可行的表示形式是一棵与语言的语法结构(运算符嵌套)相匹配的树
///        形式化语法的工作就是指定哪些字符串(由token组成的序列，也就是一个表达式)有效，哪些字符串无效。
///
/// @version 1.0.0
/// @author beacon (bquanlicn@gmail.com)
/// @date 2025-11-19
/// @copyright Copyright (c) 2025
//////////////////////////

#pragma once

#include "utils/utils.hh"

#include "token.hh"
#include "utils.hh"
#include <any>
#include <iostream>
#include <list>
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
  parse(format_parse_context& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(beacon_lox::UnaryOp const& op, FormatContext& ctx) const
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
  parse(format_parse_context& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto
  format(beacon_lox::BinaryOp const& op, FormatContext& ctx) const
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
class VarExpr;
class Assignable;

using LiteralExprPtr = std::unique_ptr<LiteralExpr>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using GroupingExprPtr = std::unique_ptr<GroupingExpr>;
using VarExprPtr = std::unique_ptr<VarExpr>;
using AssignablePtr = std::unique_ptr<Assignable>;

// 这种是编译期 多态，默认会构建为 std::monostate
using Expr = std::variant<LiteralExprPtr,
                          UnaryExprPtr,
                          BinaryExprPtr,
                          GroupingExprPtr,
                          VarExprPtr>;


class ExprBase : private Uncopyabble
{
public:
  virtual ~ExprBase() = default;
};

class LiteralExpr : private ExprBase
{
public:
  Literal literal;
  explicit LiteralExpr(Literal _literal)
    : literal(_literal)
  {}
};

class BinaryExpr : private ExprBase
{
public:
  Expr left;
  Token token;
  Expr right;
  explicit BinaryExpr(Expr _left, Token _token, Expr _right)
    : left(std::move(_left))
    , token(_token)
    , right(std::move(_right))
  {}
};

class UnaryExpr : private ExprBase
{
public:
  Token token;
  Expr expr;

  explicit UnaryExpr(Token _token, Expr _expr)
    : token(_token)
    , expr(std::move(_expr))
  {}
};

class Assignable : private ExprBase
{
public:
  Token name;
  mutable signed long distance{-1};
  mutable bool is_captured{false};
  explicit Assignable(Token const& _name)
    : name(_name)
  {}
};
class VarExpr : public Assignable
{
public:
  explicit VarExpr(Token const& _token)
    : Assignable(_token)
  {}
};

class GroupingExpr : private ExprBase
{
public:
  Expr expr;

  explicit GroupingExpr(Expr _expr)
    : expr(std::move(_expr))
  {}
};


////////////////////////////////////////////////////////
/// stmt

class ExpressionStmt;
class PrintStmt;
class VarStmt;

using ExpressionStmtPrt = std::shared_ptr<ExpressionStmt>;
using PrintStmtPrt = std::shared_ptr<PrintStmt>;
using VarStmtPrt = std::shared_ptr<VarStmt>;

using Stmt = std::variant<ExpressionStmtPrt, PrintStmtPrt, VarStmtPrt>;

using StmtList = std::list<Stmt>;

// 所以,一个表达式语句,其实也是一个表达式,然后加了 ; 么?
class ExpressionStmt : public Uncopyabble
{
public:
  Expr expr;
  explicit ExpressionStmt(Expr expression)
    : expr{std::move(expression)}
  {}
};

class PrintStmt : public Uncopyabble
{
public:
  Expr expr;
  explicit PrintStmt(Expr expression)
    : expr{std::move(expression)}
  {}
};

class VarStmt : public Uncopyabble
{
public:
  Token name;
  Expr initializer;
  explicit VarStmt(Token const& _name, Expr _initializer)
    : name(_name)
    , initializer(std::move(_initializer))
  {}
};

} // namespace beacon_lox
