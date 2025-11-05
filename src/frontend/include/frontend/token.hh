#pragma once

#include <format>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace beacon_lox
{
enum TokenType
{
  // Single-character tokens.
  // ()
  LEFT_PAREN,
  RIGHT_PAREN,
  // {}
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANS,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  LOX_EOF
};

using Literal = std::variant<std::nullptr_t, std::string_view, double, bool>;


class Token
{
public:
  // lexeme n. 词位，词素
  explicit Token(const TokenType type,
                 const std::string_view lexeme,
                 const Literal &literal,
                 const unsigned int line)
    : type_{type}
    , lexeme_{lexeme}
    , literal_{literal}
    , line_{line}
  {}

  [[nodiscard]] TokenType
  get_type() const
  {
    return type_;
  }

  [[nodiscard]] std::string_view
  get_lexeme() const
  {
    return lexeme_;
  }

  [[nodiscard]] Literal
  get_literal() const
  {
    return literal_;
  }

  [[nodiscard]] unsigned int
  get_line() const
  {
    return line_;
  }

private:
  TokenType type_;
  // 形式区分,  lexis: 词语,单词 -eme: 表示最小单位
  // 最小语义单元的原始字符串表示, 就是 token
  std::string_view lexeme_;
  // 语义, 就是值
  Literal literal_;
  unsigned int line_;
};

} // namespace beacon_lox

// 自定义格式化器
template <>
struct std::formatter<beacon_lox::Literal>
{
  constexpr auto
  parse(std::format_parse_context &ctx)
  {
    return ctx.begin();
  }

  auto
  format(const beacon_lox::Literal &literal, std::format_context &ctx) const
  {
    // visit 本身需要的也是一个函数对象, 这里使用lambda
    // 没有使用重载的 () 运算符, 而是手动返回
    return std::visit(
        [&ctx](const auto &value)
        {
          using T = std::decay_t<decltype(value)>;
          // if constexpr(std::is_same_v<T, std::nullptr_t>)
          // {
          //   return std::format_to(ctx.out(), "null");
          // }
          // else if constexpr(std::is_same_v<T, std::string_view> ||
          //                   std::is_same_v<T, double>)
          // {
          //   return std::format_to(ctx.out(), "{}", value);
          // }
          // else if constexpr(std::is_same_v<T, bool>)
          // {
          //   return std::format_to(ctx.out(), "{}", value ? "true" : "false");
          // }
          if constexpr(std::is_same_v<T, std::nullptr_t>)
          {
            return std::format_to(ctx.out(), "null");
          }
          else if constexpr(std::is_same_v<T, bool>)
          {
            return std::format_to(ctx.out(), "{:s}", value);
          }
          // 因为 value 的类型只能是这4种
          else if constexpr(std::is_same_v<T, double>)
          {
            // return std::format_to(ctx.out(), "{}", value);
            // 对于输出精度的理解...
            // if(std::abs(std::modf(value, &d)) < 1e-10)
            // {
            //   return std::format_to(ctx.out(), "{:.1f}", value);
            // }
            // 使用 std::to_string 获取完整小数表示
            std::string str = std::format("{:g}", value);
            // 移除尾部多余的 0
            // str.erase(str.find_last_not_of('0') + 1, std::string::npos);
            // 如果没有小数点，添加 .0 确保至少一位小数
            // if(str.find('.') == str.size() - 1)
            // {
            //   str += "0";
            // }
            if(str.find('.') == std::string::npos)
            {
              str += ".0"; // 确保至少 1 位小数
            }
            return std::format_to(ctx.out(), "{}", str);
          }
          else
          {
            return std::format_to(ctx.out(), "{}", value);
          }
        },
        literal);
  }
};



// 自定义格式化器
template <>
struct std::formatter<beacon_lox::Token>
{
  constexpr auto
  parse(std::format_parse_context &ctx)
  {
    return ctx.begin();
  }

  auto
  format(const beacon_lox::Token &token, std::format_context &ctx) const
  {
    // visit 本身需要的也是一个函数对象, 这里使用lambda
    // 没有使用重载的 () 运算符, 而是手动返回
    return std::format_to(ctx.out(), "{}", token.get_lexeme());
  }
};

using beacon_lox::TokenType;

// 完整的映射表
const std::unordered_map<TokenType, std::string> token_type_2_string = {
    {TokenType::LEFT_PAREN, "LEFT_PAREN"},
    {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
    {TokenType::LEFT_BRACE, "LEFT_BRACE"},
    {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::DOT, "DOT"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::PLUS, "PLUS"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::SLASH, "SLASH"},
    {TokenType::STAR, "STAR"},
    {TokenType::BANS, "BANG"},
    {TokenType::BANG_EQUAL, "BANG_EQUAL"},
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
    {TokenType::GREATER, "GREATER"},
    {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
    {TokenType::LESS, "LESS"},
    {TokenType::LESS_EQUAL, "LESS_EQUAL"},
    {TokenType::IDENTIFIER, "IDENTIFIER"},
    {TokenType::STRING, "STRING"},
    {TokenType::NUMBER, "NUMBER"},
    {TokenType::AND, "AND"},
    {TokenType::CLASS, "CLASS"},
    {TokenType::ELSE, "ELSE"},
    {TokenType::FALSE, "FALSE"},
    {TokenType::FUN, "FUN"},
    {TokenType::FOR, "FOR"},
    {TokenType::IF, "IF"},
    {TokenType::NIL, "NIL"},
    {TokenType::OR, "OR"},
    {TokenType::PRINT, "PRINT"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::SUPER, "SUPER"},
    {TokenType::THIS, "THIS"},
    {TokenType::TRUE, "TRUE"},
    {TokenType::VAR, "VAR"},
    {TokenType::WHILE, "WHILE"},
    {TokenType::LOX_EOF, "EOF"}};

// 这里已经忘记了怎么实现自定义 format 了.
template <>
class std::formatter<TokenType>
{
public:
  constexpr auto
  parse(std::format_parse_context &ctx)
  {
    return ctx.begin();
  }


  template <typename FormatContext>
  auto
  format(const TokenType &value, FormatContext &context) const
  {
    auto it = token_type_2_string.find(value);
    std::string str =
        (it == token_type_2_string.end()) ? "Unkonwn" : it->second;
    return std::format_to(context.out(), "{}", str);
  }
};