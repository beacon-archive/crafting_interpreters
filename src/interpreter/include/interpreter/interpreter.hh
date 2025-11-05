#pragma once

#include <iostream>

#include "frontend/ast.hh"
#include "frontend/error.hh"

namespace beacon_lox
{

class Interpreter : public Visitor
{
public:
  void
  interpret(const Expr &expr)
  {
    try
    {
      auto value = evaluate(expr);
      std::cout << "result: " << stringify(value) << "\n";
    }
    catch(const Error::RuntimeError &e)
    {
      // std::cout << "catch exception: " << e.what() << "\n";
      runtime_error(e);
    }
  }

  std::any
  literal_expr_visitor(LiteralExpr *literal) override
  {
    // std::cout << "literal_expr_visitor::literal->literal type:" << literal->literal.index() << "\n";
    // // 这样返回的话, 这里返回的是一个 variant 对象, 而非一个double 类型数值.
    // !它被自动包装为 std::any 存储一个 std::variant 对象
    // return literal->literal;
    // // return 3.0;

    // std::cout << "literal_expr_visitor::literal->literal index:"
    //           << literal->literal.index() << "\n";

    // 使用 std::visit 提取并返回具体值
    return std::visit(
        [](const auto &value) -> std::any
        {
          // 所有类型都可以直接包装到 std::any
          return std::any{value};
        },
        literal->literal);
  }

  std::any
  unary_expr_visitor(UnaryExpr *unary) override
  {
    auto value = evaluate(unary->expr);
    switch(unary->op)
    {
      case UnaryOp::MINUS:
        try
        {
          check_number_operand(unary->token, value);
          return -std::any_cast<double>(value);
        }
        catch(std::exception &e)
        {
          // std::cout << "value: " << std::any_cast<std::string>(value)
          //           << "\n";
          throw Error::RuntimeError(unary->token, "unary minus must be number");
        }
      case UnaryOp::BANS:
        // 这里的关键点是, 在 lox 中除了 ture, 其它的都是 false
        return !is_true(value);
      default:
        // throw Error::RuntimeError(unary->token, "unkown unary operator");
        std::cout << "runtime error\n";
        return "runtime error";
    }
  }
  std::any
  binary_expr_visitor(BinaryExpr *binary) override
  {
    auto left = evaluate(binary->left);
    auto right = evaluate(binary->right);
    switch(binary->op)
    {
      case BinaryOp::PLUS:
        if(is_type<double>(left) && is_type<double>(right))
        {
          return std::any_cast<double>(left) + std::any_cast<double>(right);
        }
        if(is_type<std::string>(left) && is_type<std::string>(right))
        {
          return std::any_cast<std::string>(left) +
                 std::any_cast<std::string>(right);
        }
        throw Error::RuntimeError(binary->token, "oprand must be two strings!");
        break;
      case BinaryOp::BANG_EQUAL:
        return !is_equal(left, right);
      case BinaryOp::EQUAL_EQUAL:
        return is_equal(left, right);
      case BinaryOp::MINUS:
        check_number_operand(binary->token, left, right);
        return std::any_cast<double>(left) - std::any_cast<double>(right);
      case BinaryOp::SLASH:
        check_number_operand(binary->token, left, right);
        return std::any_cast<double>(left) / std::any_cast<double>(right);
      case BinaryOp::STAR:
        check_number_operand(binary->token, left, right);
        return std::any_cast<double>(left) * std::any_cast<double>(right);
      case BinaryOp::GREATER:
        check_number_operand(binary->token, left, right);
        return std::any_cast<double>(left) > std::any_cast<double>(right);
      case BinaryOp::GREATER_EQUAL:
        check_number_operand(binary->token, left, right);
        return std::any_cast<double>(left) >= std::any_cast<double>(right);
      case BinaryOp::LESS:
        check_number_operand(binary->token, left, right);
        return std::any_cast<double>(left) < std::any_cast<double>(right);
      case BinaryOp::LESS_EQUAL:
        check_number_operand(binary->token, left, right);
        return std::any_cast<double>(left) <= std::any_cast<double>(right);
        break;
    }
    return true;
  }
  std::any
  grouping_expr_visitor(GroupingExpr *grouping) override
  {
    return evaluate(grouping->expr);
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

private:
  std::any
  evaluate(const Expr &expr)
  {
    // 这里忘记了 variant 的 visit 访问方法, accept 并不是 variant 的, 而是里面的值的
    // return expr.accept(this);
    return std::visit([this](const auto &value) { return value->accept(this); },
                      expr);
  }

  std::string
  stringify(std::any any)
  {
    if(is_type<std::nullptr_t>(any))
    {
      return "nil";
    }

    if(is_type<double>(any))
    {
      return std::to_string(std::any_cast<double>(any));
    }

    if(is_type<bool>(any))
    {
      return std::any_cast<bool>(any) ? "ture" : "false";
    }

    return std::any_cast<std::string>(any);
  }

  // 除了 nullptr 或者 false, 其它任何的东西都是 true
  bool
  is_true(std::any value)
  {
    if(is_type<std::nullptr_t>(value))
    {
      return false;
    }

    if(is_type<bool>(value))
    {
      return std::any_cast<bool>(value);
    }

    return true;
  }

  // bool
  // is_equal(const std::any &left, const std::any &right)
  // {
  //   if(is_type<std::nullptr_t>(left) && is_type<std::nullptr_t>(right))
  //   {
  //     return true;
  //   }
  //   if(is_type<std::nullptr_t>(left))
  //   {
  //     return false;
  //   }
  //   return left == right;
  // }

  bool
  is_equal(const std::any &left, const std::any &right)
  {
    // 1. 两个都是空值（std::nullptr_t）
    if(is_type<std::nullptr_t>(left) && is_type<std::nullptr_t>(right))
    {
      return true;
    }
    // 2. 只有一个为空值
    if(is_type<std::nullptr_t>(left) || is_type<std::nullptr_t>(right))
    {
      return false;
    }
    // 3. 类型不同
    if(left.type() != right.type())
    {
      return false;
    }
    // 4. 类型相同，比较值
    // 为了支持多种类型，这里需要针对具体类型进行 any_cast 并比较
    // 示例：假设支持 int、double、std::string 类型
    try
    {
      if(is_type<double>(left))
      {
        return std::any_cast<double>(left) == std::any_cast<double>(right);
      }
      if(is_type<std::string>(left))
      {
        return std::any_cast<std::string>(left) ==
               std::any_cast<std::string>(right);
      }
      // 可以根据需要添加更多类型支持
      // 如果类型不支持比较，抛出异常或返回 false
      return false;
    }
    catch(const std::bad_any_cast &)
    {
      return false; // 转换失败时返回 false
    }
  }

  template <typename T>
  bool
  is_type(const std::any &any)
  {
    return any.type() == typeid(T);
  }

  void
  check_number_operand(const Token &token, const std::any &operand)
  {
    if(is_type<double>(operand))
    {
      return;
    }

    throw Error::RuntimeError(token, "oprand must be a number!");
  }

  void
  check_number_operand(const Token &token,
                       const std::any &left,
                       const std::any &right)
  {
    if(is_type<double>(left) && is_type<double>(right))
    {
      return;
    }

    throw Error::RuntimeError(token, "oprand must be two numbers!");
  }

  void
  runtime_error(const Error::RuntimeError &rerr)
  {
    std::cout << std::format("{} [line:{}]\n",
                             rerr.what(),
                             rerr._token.get_line());
    had_runtime_error_ = true;
  }

  bool had_runtime_error_{false};
  bool had_error_{false};
};
} // namespace beacon_lox