#pragma once

#include "frontend/ast.hh"
#include "frontend/error.hh"

#include "interpreter/lox_object.hh"
#include "interpreter/environment.hh"



#include "utils/logger.hh"

#include <iostream>

namespace beacon_lox
{

class Interpreter
{
public:
  void
  interpret(Expr const& expr)
  {
    try
    {
      auto value = evaluate(expr);
      SPDLOG_DEBUG("result: {}", stringify(value));
    }
    catch(Error::RuntimeError const& e)
    {
      runtime_error(e);
    }
  }

  void
  interpret_stmt(StmtList const& statements)
  {
    try
    {
      for(auto const& item : statements)
      {
        execute(item);
      }
    }
    catch(std::exception const& e)
    {
      SPDLOG_ERROR("{}", e.what());
    }
  }



  [[nodiscard]] bool
  had_runtime_error() const
  {
    return had_runtime_error_;
  }

  [[nodiscard]] bool
  had_error() const
  {
    return had_error_;
  }

  // void
  // expression_stmt_visitor(Stmt stmt)
  // {
  //   if(std::shared_ptr<beacon_lox::ExpressionStmt> const* p =
  //          std::get_if<ExpressionStmtPrt>(&stmt))
  //   {
  //     // p 的类型是 std::shared_ptr<beacon_lox::ExpressionStmt> const *
  //     // 自己突然有个疑问,为什么这里不能直接访问 expr ....
  //     // 这里自己之前以为是一个裸指针,实际上并不是...
  //     // auto value = evaluate(p->expr);
  //     evaluate(p->get()->expr);
  //   }
  // }
  void
  operator()(ExpressionStmtPrt const& expr_stmt);
  void
  operator()(PrintStmtPrt const& print_stmt);
  void
  operator()(VarStmtPrt const& expr_stmt);
  auto
  operator()(LiteralExprPtr const& liter) -> LoxObject;
  auto
  operator()(UnaryExprPtr const& unary) -> LoxObject;
  auto
  operator()(BinaryExprPtr const& binary) -> LoxObject;
  auto
  operator()(GroupingExprPtr const& /*unary*/) -> LoxObject;
  auto
  operator()(VarExprPtr const& /*unary*/) -> LoxObject;

private:
  auto
  evaluate(Expr const& expr) -> LoxObject
  {
    // 这里忘记了 variant 的 visit 访问方法, accept 并不是 variant 的, 而是里面的值的
    // return expr.accept(this);
    return std::visit(*this, expr);
  }

  void
  execute(Stmt stmt)
  {
    std::visit(*this, stmt);
  }

  std::string
  stringify(std::any any);

  // 除了 nullptr 或者 false, 其它任何的东西都是 true
  bool
  is_true(std::any value);

  // 这里的类型比较值得自己认真学习一下～
  bool
  is_equal(std::any const& left, std::any const& right);

  template <typename T>
  bool
  is_type(std::any const& any);
  void
  check_number_operand(Token const& token, std::any const& operand);
  void
  check_number_operand(Token const& token,
                       std::any const& left,
                       std::any const& right);

  void
  runtime_error(Error::RuntimeError const& rerr);

  bool had_runtime_error_{false};
  bool had_error_{false};
  EnvironmentPrt global_{std::make_shared<Environment>()};
  EnvironmentPrt environment_{global_};
};
} // namespace beacon_lox



//////////////////////////////////////////////////////////////////
// std::any
// literal_expr_visitor(LiteralExpr* literal)
// {
//   // std::cout << "literal_expr_visitor::literal->literal type:" << literal->literal.index() << "\n";
//   // // 这样返回的话, 这里返回的是一个 variant 对象, 而非一个double 类型数值.
//   // !它被自动包装为 std::any 存储一个 std::variant 对象
//   // return literal->literal;
//   // // return 3.0;
//   // std::cout << "literal_expr_visitor::literal->literal index:"
//   //           << literal->literal.index() << "\n";
//   // 使用 std::visit 提取并返回具体值
//   // 所有类型都可以直接包装到 std::any
//   // 这样的话,返回的类型就不是统一的 string 了,可能是double,可能是string...
//   // return std::visit([](auto const& value) -> std::any
//   //                   { return std::any{value}; },
//   //                   literal->literal);
//   return std::visit(Overloaded{[](double const& value) -> std::any
//                                { return std::to_string(value); },
//                                [](bool const& value) -> std::any
//                                { return value ? "true" : "false"; },
//                                [](std::string_view const& value) -> std::any
//                                { return std::string(value); },
//                                [](auto const& value) -> std::any
//                                { return std::any{value}; }},
//                     literal->literal);
// }

// std::any
// unary_expr_visitor(UnaryExpr* unary)
// {
//   auto value = evaluate(unary->expr);
//   switch(unary->token.get_type())
//   {
//     case TokenType::MINUS:
//       try
//       {
//         check_number_operand(unary->token, value);
//         return -std::any_cast<double>(value);
//       }
//       catch(std::exception& e)
//       {
//         // std::cout << "value: " << std::any_cast<std::string>(value)
//         //           << "\n";
//         throw Error::RuntimeError(unary->token, "unary minus must be number");
//       }
//     case TokenType::BANS:
//       // 这里的关键点是, 在 lox 中除了 false 和 nil 是假，其它的都是 true
//       return !is_true(value);
//     default:
//       SPDLOG_ERROR("runtime error, unknown token type");
//       throw Error::RuntimeError(unary->token, "unkown unary operator");
//       // return "runtime error";
//   }
// }

// std::any
// binary_expr_visitor(BinaryExpr* binary)
// {
//   auto left = evaluate(binary->left);
//   auto right = evaluate(binary->right);
//   switch(binary->token.get_type())
//   {
//     case TokenType::PLUS:
//       if(is_type<double>(left) && is_type<double>(right))
//       {
//         return std::any_cast<double>(left) + std::any_cast<double>(right);
//       }
//       if(is_type<std::string>(left) && is_type<std::string>(right))
//       {
//         return std::any_cast<std::string>(left) +
//                std::any_cast<std::string>(right);
//       }
//       throw Error::RuntimeError(binary->token, "oprand must be two strings!");
//       break;
//     case TokenType::BANG_EQUAL:
//       return !is_equal(left, right);
//     case TokenType::EQUAL_EQUAL:
//       return is_equal(left, right);
//     case TokenType::MINUS:
//       check_number_operand(binary->token, left, right);
//       return std::any_cast<double>(left) - std::any_cast<double>(right);
//     case TokenType::SLASH:
//       check_number_operand(binary->token, left, right);
//       return std::any_cast<double>(left) / std::any_cast<double>(right);
//     case TokenType::STAR:
//       check_number_operand(binary->token, left, right);
//       return std::any_cast<double>(left) * std::any_cast<double>(right);
//     case TokenType::GREATER:
//       check_number_operand(binary->token, left, right);
//       return std::any_cast<double>(left) > std::any_cast<double>(right);
//     case TokenType::GREATER_EQUAL:
//       check_number_operand(binary->token, left, right);
//       return std::any_cast<double>(left) >= std::any_cast<double>(right);
//     case TokenType::LESS:
//       check_number_operand(binary->token, left, right);
//       return std::any_cast<double>(left) < std::any_cast<double>(right);
//     case TokenType::LESS_EQUAL:
//       check_number_operand(binary->token, left, right);
//       return std::any_cast<double>(left) <= std::any_cast<double>(right);
//       break;
//       // 剩下的那些，可以先不考虑
//   }
//   return true;
// }
// std::any
// grouping_expr_visitor(GroupingExpr* grouping)
// {
//   return evaluate(grouping->expr);
// }
// std::any
// var_expr_visitor(VarExpr* /*var*/)
// {
//   // return evaluate(var->expr);
//   return {};
// }
// void
// print_stmt_visitor(Stmt const& stmt)
// {
//   // 返回的p指向的是 PrintStmtPrt 的指针
//   // 所以 p 是一个二级指针! 它不是 std::shared_ptr<PrintStmt>,而是指向这个的指针!
//   if(auto const* p = std::get_if<PrintStmtPrt>(&stmt))
//   {
//     // 自己突然有个疑问,为什么这里不能直接访问 expr ....
//     // auto value = evaluate(p->expr);
//     // if(*p == nullptr)
//     // {
//     //   SPDLOG_ERROR("sssssss");
//     //   return;
//     // }
//     auto value = evaluate(p->get()->expr);
//     std::cout << to_string(value) << "\n";
//   }
// }
