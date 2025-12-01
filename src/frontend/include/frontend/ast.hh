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

#include "token.hh"
#include "utils.hh"
#include <any>
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


// Overload 把多个 lambda 合并到一个对象中，并把它们的 operator() 全部引入到对象中。visit 调用这个对象的重载 operator()，因此根据类型自动选中正确的 lambda。
// visit 的函数签名:
//                visit(_Visitor&& __visitor, _Variants&&... __variants)
// 本身就是传入一个访问器,然后调用访问对应的()操作符来处理对应的类型,或者传入lambda
template <class... Ts>
struct Overloaded : Ts...
{
  using Ts::operator()...;
};
template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;



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

// 这种是编译期 多态，默认会构建为 std::monostate
using Expr = std::variant<std::monostate,
                          LiteralExprPtr,
                          UnaryExprPtr,
                          BinaryExprPtr,
                          GroupingExprPtr,
                          VarExprPtr>;

class Visitor
{
public:
  virtual ~Visitor() = default;
  virtual std::any
  literal_expr_visitor(LiteralExpr*) = 0;
  virtual std::any
  unary_expr_visitor(UnaryExpr*) = 0;
  virtual std::any
  binary_expr_visitor(BinaryExpr*) = 0;
  virtual std::any
  grouping_expr_visitor(GroupingExpr*) = 0;
  virtual std::any
  var_expr_visitor(VarExpr*) = 0;
  virtual std::any
  assian_expr_visitor(Assignable* /*unused*/)
  {
    return {};
  }
};

class ExprBase : private Uncopyabble
{
public:
  virtual ~ExprBase() = default;
  virtual std::any
  accept(Visitor* visitor) = 0;
};

class LiteralExpr : private ExprBase
{
public:
  Literal literal;
  explicit LiteralExpr(Literal _literal)
    : literal(_literal)
  {}

  std::any
  accept(Visitor* visitor) override
  {
    return visitor->literal_expr_visitor(this);
  }
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

  std::any
  accept(Visitor* visitor) override
  {
    return visitor->binary_expr_visitor(this);
  }
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
  std::any
  accept(Visitor* visitor) override
  {
    return visitor->unary_expr_visitor(this);
  }
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
  std::any
  accept(Visitor* visitor) override
  {
    return visitor->assian_expr_visitor(this);
  }
};
class VarExpr : Assignable
{
public:
  explicit VarExpr(Token const& _token)
    : Assignable(_token)
  {}
  std::any
  accept(Visitor* visitor) override
  {
    return visitor->var_expr_visitor(this);
  }
};

class GroupingExpr : private ExprBase
{
public:
  Expr expr;

  explicit GroupingExpr(Expr _expr)
    : expr(std::move(_expr))
  {}

  std::any
  accept(Visitor* visitor) override
  {
    return visitor->grouping_expr_visitor(this);
  }
};

class ExprVisitor : public Visitor
{
public:
  std::any
  literal_expr_visitor(LiteralExpr* literal) override
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
  unary_expr_visitor(UnaryExpr* unary) override
  {
    return std::format(
        "({} {} {})",
        unary->token.get_type(),
        unary->token.get_lexeme(),
        std::visit(
            Overloaded{
                // 专门处理 monostate 的情况
                [](std::monostate) -> std::string { return "nil"; },
                // 处理其他所有情况 (指针类型)
                [this](auto const& val) -> std::string
                { return std::any_cast<std::string>(val->accept(this)); }},
            unary->expr));
  }

  std::any
  binary_expr_visitor(BinaryExpr* binary) override
  {
    return std::format(
        "({} {} {} {})",
        binary->token.get_type(),
        binary->token.get_lexeme(),
        std::visit(
            Overloaded{
                // 专门处理 monostate 的情况
                [](std::monostate) -> std::string { return "nil"; },
                // 处理其他所有情况 (指针类型)
                [this](auto const& val) -> std::string
                { return std::any_cast<std::string>(val->accept(this)); }},
            binary->left),
        std::visit(
            Overloaded{
                // 专门处理 monostate 的情况
                [](std::monostate) -> std::string { return "nil"; },
                // 处理其他所有情况 (指针类型)
                [this](auto const& val) -> std::string
                { return std::any_cast<std::string>(val->accept(this)); }},
            binary->right));
  }

  std::any
  grouping_expr_visitor(GroupingExpr* grouping) override
  {
    return std::format(
        "(grouping {})",
        std::visit(
            Overloaded{
                // 专门处理 monostate 的情况
                [](std::monostate) -> std::string { return "nil"; },
                // 处理其他所有情况 (指针类型)
                [this](auto const& val) -> std::string
                { return std::any_cast<std::string>(val->accept(this)); }},
            grouping->expr));
  }
};


////////////////////////////////////////////////////////
/// stmt

class ExpressionStmt;
class PrintStmt;
class VarStmt;

using ExpressionStmtPrt = std::shared_ptr<ExpressionStmt>;
using PrintStmtPrt = std::shared_ptr<PrintStmt>;
using VarStmtPrt = std::shared_ptr<VarStmt>;

using Stmt =
    std::variant<std::monostate, ExpressionStmtPrt, PrintStmtPrt, VarStmtPrt>;

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
