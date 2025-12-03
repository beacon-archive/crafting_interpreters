#include "interpreter/interpreter.hh"
#include "utils/utils.hh"


namespace beacon_lox
{
void
Interpreter::operator()(PrintStmtPrt const& print_stmt)
{
  // 返回的p指向的是 PrintStmtPrt 的指针
  // 所以 p 是一个二级指针! 它不是 std::shared_ptr<PrintStmt>,而是指向这个的指针!

  // 自己突然有个疑问,为什么这里不能直接访问 expr ....
  // auto value = evaluate(p->expr);
  // if(*p == nullptr)
  // {
  //   SPDLOG_ERROR("sssssss");
  //   return;
  // }
  auto value = evaluate(print_stmt->expr);
  std::cout << to_string(value) << "\n";
}

void
Interpreter::operator()(ExpressionStmtPrt const& expr_stmt)
{
  evaluate(expr_stmt->expr);
}

void
Interpreter::operator()(VarStmtPrt const& expr_stmt)
{
  LoxObject const value = evaluate(expr_stmt->initializer);
  environment_->define(expr_stmt->name.get_lexeme(), value);
}

auto
Interpreter::operator()(LiteralExprPtr const& liter) -> LoxObject
{
  return std::visit(
      Overloaded{[](double value) -> LoxObject { return LoxObject{value}; },
                 [](std::string_view value) -> LoxObject
                 { return LoxObject{std::string(value)}; },
                 [](bool value) -> LoxObject
                 { return value ? LoxObject("true") : LoxObject{"false"}; },
                 [](auto const&) -> LoxObject { return {}; }},
      liter->literal);
}

auto
Interpreter::operator()(UnaryExprPtr const& unary) -> LoxObject
{
  auto value = evaluate(unary->expr);
  switch(unary->token.get_type())
  {
    case TokenType::MINUS:
      if(std::holds_alternative<LoxDouble>(value))
      {
        return -std::get<LoxDouble>(value);
      }
      throw Error::RuntimeError(unary->token, "unary minus must be number");
      break;
    case TokenType::BANS:
      // 这里的关键点是, 在 lox 中除了 false 和 nil 是假，其它的都是 true
      return std::visit(Overloaded{[](auto const&) -> bool { return true; },
                                   [](bool value) { return value; },
                                   [](std::nullptr_t /*value*/)
                                   { return false; }},
                        value);
    default:
      SPDLOG_ERROR("runtime error, unknown token type");
      throw Error::RuntimeError(unary->token, "unkown unary operator");
      // return "runtime error";
  }

  return std::visit(
      Overloaded{[](double value) -> LoxObject { return LoxObject{value}; },
                 [](std::string_view value) -> LoxObject
                 { return LoxObject{std::string(value)}; },
                 [](bool value) -> LoxObject
                 { return value ? LoxObject("true") : LoxObject{"false"}; },
                 [](auto const&) -> LoxObject { return {}; }},
      unary->expr);
}

auto
Interpreter::operator()(BinaryExprPtr const& binary) -> LoxObject
{
  auto const& left = evaluate(binary->left);

  auto const& right = evaluate(binary->right);

  switch(binary->token.get_type())
  {
    case TokenType::PLUS:
      if(std::holds_alternative<LoxDouble>(left) &&
         std::holds_alternative<LoxDouble>(right))
      {
        return std::get<LoxDouble>(left) + std::get<LoxDouble>(right);
      }
      if(std::holds_alternative<LoxString>(left) &&
         std::holds_alternative<LoxString>(right))
      {
        return std::get<LoxString>(left) + std::get<LoxString>(right);
      }
      throw Error::RuntimeError(binary->token,
                                "Operands must be two strings or two numbers!");
    case TokenType::BANG_EQUAL:
      return !is_equal(left, right);
    case TokenType::EQUAL_EQUAL:
      return is_equal(left, right);
    case TokenType::MINUS:
      check_number_operand(binary->token, left, right);
      return std::get<LoxDouble>(left) - std::get<LoxDouble>(right);
    case TokenType::SLASH:
      check_number_operand(binary->token, left, right);
      return std::get<LoxDouble>(left) / std::get<LoxDouble>(right);
    case TokenType::STAR:
      check_number_operand(binary->token, left, right);
      return std::get<LoxDouble>(left) * std::get<LoxDouble>(right);
    case TokenType::GREATER:
      check_number_operand(binary->token, left, right);
      return std::get<LoxDouble>(left) > std::get<LoxDouble>(right);
    case TokenType::GREATER_EQUAL:
      check_number_operand(binary->token, left, right);
      return std::get<LoxDouble>(left) >= std::get<LoxDouble>(right);
    case TokenType::LESS:
      check_number_operand(binary->token, left, right);
      return std::get<LoxDouble>(left) < std::get<LoxDouble>(right);
    case TokenType::LESS_EQUAL:
      check_number_operand(binary->token, left, right);
      return std::get<LoxDouble>(left) <= std::get<LoxDouble>(right);
      break;
    default:
      break;
      // 剩下的那些，可以先不考虑
  }
  return true;
}

auto
Interpreter::operator()(GroupingExprPtr const& /*unary*/) -> LoxObject
{
  return LoxObject{nullptr};
}

auto
Interpreter::operator()(VarExprPtr const& var_expr) -> LoxObject
{
  return environment_->get(var_expr.get()->name);
}

bool
Interpreter::is_equal(LoxObject const& left, LoxObject const& right)
{
  // 1. 两个都是空值（std::nullptr_t）
  if(std::holds_alternative<std::nullptr_t>(left) &&
     std::holds_alternative<std::nullptr_t>(right))
  {
    return true;
  }
  // 2. 只有一个为空值
  if(std::holds_alternative<std::nullptr_t>(left) ||
     std::holds_alternative<std::nullptr_t>(right))
  {
    return false;
  }
  // 3. 类型不同
  if(left.index() != right.index())
  {
    return false;
  }
  // 4. 类型相同，比较值
  // 为了支持多种类型，这里需要针对具体类型进行 any_cast 并比较
  // 示例：假设支持 int、double、std::string 类型

  if(std::holds_alternative<double>(left))
  {
    return std::get<LoxDouble>(left) == std::get<LoxDouble>(right);
  }
  if(std::holds_alternative<std::string>(left))
  {
    return std::get<LoxString>(left) == std::get<LoxString>(right);
  }
  // 其它未知类型
  return false; // 转换失败时返回 false
}

template <typename T>
bool
Interpreter::is_type(std::any const& any)
{
  return any.type() == typeid(T);
}

void
Interpreter::check_number_operand(Token const& token, LoxObject const& operand)
{
  if(std::holds_alternative<LoxDouble>(operand))
  {
    return;
  }

  throw Error::RuntimeError(token, "oprand must be a number!");
}

void
Interpreter::check_number_operand(Token const& token,
                                  LoxObject const& left,
                                  LoxObject const& right)
{
  if(std::holds_alternative<LoxDouble>(left) &&
     std::holds_alternative<LoxDouble>(right))
  {
    return;
  }

  throw Error::RuntimeError(token, "oprand must be two numbers!");
}

void
Interpreter::runtime_error(Error::RuntimeError const& rerr)
{
  std::cout << std::format("{} [line:{}]\n",
                           rerr.what(),
                           rerr._token.get_line());
  had_runtime_error_ = true;
}

// 除了 nullptr 或者 false, 其它任何的东西都是 true
// bool
// Interpreter::is_true(std::any value)
// {
//   if(is_type<std::nullptr_t>(value))
//   {
//     return false;
//   }

//   if(is_type<bool>(value))
//   {
//     return std::any_cast<bool>(value);
//   }

//   return true;
// }


} // namespace beacon_lox