#pragma once

#include "token.hh"

#include "utils/logger.hh"

#include <vector>
#include <string>
#include <string_view>


namespace beacon_lox
{
//////////////////////////
/// @brief 以字符串形式接收原始源代码，并将其转换为一个稍高级别的表示：一系列词法标记(Token)。
///        这里的语法是: 词汇语法(字符如何被分组为词法标记的规则)，这种形式体系，被称为正则语言。
///          这对于我们的扫描器来说没什么问题，因为它输出的是一个扁平的词法标记序列。
///          但正则语言还不够强大，无法处理可以任意深度嵌套的表达式。
/// @date 2025-11-19
//////////////////////////
class Scanner
{
public:
  explicit Scanner(std::string contents)
    : source_(std::move(contents))
  {}

  std::vector<Token>&
  scan_tokens()
  {
    while(!is_end())
    {
      start_ = cur_;
      // 进行扫描时，需要先获取一个 char
      scan_token();
    }
    tokens_.emplace_back(TokenType::LOX_EOF, "", "null", line_);
    return tokens_;
  }

private:
  //////////////////////////
  /// @brief 添加一个 token，没有字面量的 token
  ///
  /// @param type
  /// @date 2025-11-18
  //////////////////////////
  void
  add_token(TokenType type)
  {
    add_token(type, nullptr);
  }

  //////////////////////////
  /// @brief
  ///
  /// @param type
  /// @param literal  是当前 token 解析后得到的值， ？ 看了下书，说的是字面量
  /// @date 2025-11-17
  //////////////////////////
  void
  add_token(TokenType type, Literal literal)
  {
    // 注意,这里的 cur_, start_ 和自己写时的区别
    // 现在自己猜想,应该是这里创建了局部的 std::string, 导致离开这个函数后,被释放掉了
    // 这个存储的是原始字符值
    auto lexeme =
        static_cast<std::string_view>(source_).substr(start_, cur_ - start_);
    // 这里自己一开始想用 get 方法来获取, 但是 get 方法需要的是一个编译期常量, 所以失败了
    // std::cout << "add token lexeme:" << lexeme << ", literal: ";
    // std::visit([](const auto &value) { std::cout << value; }, literal);
    // std::cout << "\n";

    tokens_.emplace_back(type, lexeme, literal, line_);
  }

  //////////////////////////
  /// @brief  操作符既不是字符串,也不是数值,也不是布尔类型
  ///
  /// @date 2025-11-17
  //////////////////////////
  void
  scan_token();

  void
  string();

  void
  number();

  //////////////////////////
  /// @brief  这里有个很重要的一点，关键字是标识符的一种，
  ///            同时标识符并不需要真的知道它的名字是什么，对于第二个持疑问态度。
  ///
  /// @date 2025-11-19
  //////////////////////////
  void
  identifier();

  //////////////////////////
  /// @brief  当匹配到时, 需要读取 | 相当于一个有条件的 advance
  ///
  /// @param c
  /// @return true
  /// @return false
  /// @date 2025-11-18
  //////////////////////////
  bool
  match(char c)
  {
    if(is_end())
    {
      return false;
    }
    // if(source_[cur_] == c)
    // {
    //   advance();
    //   return true;
    // }
    // return false;
    // 不等于就不消费
    if(source_[cur_] != c)
    {
      return false;
    }
    ++cur_;
    return true;
  }

  static bool
  is_alpha(char c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }
  static bool
  is_digit(char c)
  {
    return c >= '0' && c <= '9';
  }
  static bool
  is_alpha_digit(char c)
  {
    return is_alpha(c) || is_digit(c);
  }

  // 自己写代码时,实际上还是没有想清楚, peek 和 advance 针对的是同一个字符串
  // 只是一个是访问, 一个不访问...
  // 这个更好的理解是消费！消费一个字符
  char
  advance()
  {
    return source_[cur_++];
  }
  //////////////////////////
  /// @brief  这个也就是类似于： lookahead
  ///
  /// @return char
  /// @date 2025-11-18
  //////////////////////////
  [[nodiscard]] char
  peek() const
  {
    if(is_end())
    {
      return '\0';
    }

    return source_[cur_];
  }
  [[nodiscard]] char
  peek_next() const
  {
    if(cur_ + 1 > source_.size())
    {
      return '\0';
    }

    return source_[cur_ + 1];
  }

  [[nodiscard]] bool
  is_end() const
  {
    return cur_ >= source_.size();
  }

  // 表示原始代码字符串
  std::string source_;
  // 跟踪的是 cur_ 所指向的字符所在的源文件行数
  unsigned long int line_{1};
  // 指向的是当前被扫描的词素中的第一个字符
  unsigned long int start_{0};
  // 指向的是当前正在处理的字符
  unsigned long int cur_{0};
  std::string_view contents_;
  std::vector<Token> tokens_;
};
} // namespace beacon_lox