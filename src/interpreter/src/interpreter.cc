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
      try
      {
        check_number_operand(unary->token, value);
        return -std::any_cast<double>(value);
      }
      catch(std::exception& e)
      {
        // std::cout << "value: " << std::any_cast<std::string>(value)
        //           << "\n";
        throw Error::RuntimeError(unary->token, "unary minus must be number");
      }
    case TokenType::BANS:
      // 这里的关键点是, 在 lox 中除了 false 和 nil 是假，其它的都是 true
      return !is_true(value);
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
      if(is_type<double>(left) && is_type<double>(right))
      {
        return std::get<LoxDouble>(left) + std::get<LoxDouble>(right);
      }
      if(is_type<std::string>(left) && is_type<std::string>(right))
      {
        return std::get<LoxString>(left) + std::get<LoxString>(right);
      }
      throw Error::RuntimeError(binary->token,
                                "Operands must be two strings or two numbers!");
      break;
    case TokenType::BANG_EQUAL:
      return !is_equal(left, right);
    case TokenType::EQUAL_EQUAL:
      return is_equal(left, right);
    case TokenType::MINUS:
      check_number_operand(binary->token, left, right);
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::SLASH:
      check_number_operand(binary->token, left, right);
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
      check_number_operand(binary->token, left, right);
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    case TokenType::GREATER:
      check_number_operand(binary->token, left, right);
      return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
      check_number_operand(binary->token, left, right);
      return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case TokenType::LESS:
      check_number_operand(binary->token, left, right);
      return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
      check_number_operand(binary->token, left, right);
      return std::any_cast<double>(left) <= std::any_cast<double>(right);
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
Interpreter::operator()(VarExprPtr const& /*unary*/) -> LoxObject
{
  return LoxObject{nullptr};
}

bool
Interpreter::is_equal(std::any const& left, std::any const& right)
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
  catch(std::bad_any_cast const&)
  {
    return false; // 转换失败时返回 false
  }
}

template <typename T>
bool
Interpreter::is_type(std::any const& any)
{
  return any.type() == typeid(T);
}

void
Interpreter::check_number_operand(Token const& token, std::any const& operand)
{
  if(is_type<double>(operand))
  {
    return;
  }

  throw Error::RuntimeError(token, "oprand must be a number!");
}

void
Interpreter::check_number_operand(Token const& token,
                                  std::any const& left,
                                  std::any const& right)
{
  if(is_type<double>(left) && is_type<double>(right))
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



std::string
Interpreter::stringify(std::any any)
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
Interpreter::is_true(std::any value)
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


} // namespace beacon_lox