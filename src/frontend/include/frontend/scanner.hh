#pragma once

#include "token.hh"

#include <iostream>
#include <vector>
#include <string>
#include <string_view>


namespace beacon_lox
{
class Scanner
{
public:
  explicit Scanner(std::string contents)
    : source_(std::move(contents))
  {}

  std::vector<Token> &
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
  void
  add_token(TokenType type)
  {
    add_token(type, nullptr);
  }

  void
  add_token(TokenType type, Literal literal)
  {
    // 注意,这里的 cur_, start_ 和自己写时的区别
    // 现在自己猜想,应该是这里创建了局部的 std::string, 导致离开这个函数后,被释放掉了
    auto lexeme =
        static_cast<std::string_view>(source_).substr(start_, cur_ - start_);
    // 这里自己一开始想用 get 方法来获取, 但是 get 方法需要的是一个编译期常量, 所以失败了
    // std::cout << "add token lexeme:" << lexeme << ", literal: ";
    // std::visit([](const auto &value) { std::cout << value; }, literal);
    // std::cout << "\n";

    tokens_.emplace_back(type, lexeme, literal, line_);
  }

  void
  scan_token()
  {
    switch(char c = advance())
    {
      case '(':
        add_token(TokenType::LEFT_PAREN);
        break;
      case ')':
        add_token(TokenType::RIGHT_PAREN);
        break;
      case '{':
        add_token(TokenType::LEFT_BRACE);
        break;
      case '}':
        add_token(TokenType::RIGHT_BRACE);
        break;
      case ',':
        add_token(TokenType::COMMA);
        break;
      case '.':
        add_token(TokenType::DOT);
        break;
      case '-':
        add_token(TokenType::MINUS);
        break;
      case '+':
        add_token(TokenType::PLUS);
        break;
      case ';':
        add_token(TokenType::SEMICOLON);
        break;
      case '*':
        add_token(TokenType::STAR);
        break;
      case '!':
        add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANS);
        break;
      case '=':
        add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
      case '<':
        add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
      case '>':
        add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
      case '\n':
        ++line_;
        break;
      case ' ':
      case '\t':
      case 'r':
        break;
      case '/':
        if(match('/'))
        {
          // while(match('\n'))
          while(!match('\n') && !is_end())
          {
            advance();
          }
        }
        else
        {
          add_token(TokenType::SLASH);
        }
        break;
      case '"':
        string();
        break;
      default:
        if(is_digit(c))
        {
          number();
        }
        else if(is_alpha(c))
        {
          identifier();
        }
        else
        {
          std::cerr << "unknown char:" << c << "\n";
        }
        break;
    }
  }

  void
  string()
  {
    while(peek() != '"' && !is_end())
    {
      if(peek() == '\n')
      {
        ++line_;
      }
      advance();
    }

    if(is_end())
    {
      // lex: error
      return;
    }

    // 忘记了后引号
    advance();
    //                                        [start, end)   左闭右开
    // 今天对于这个 substr 有了更深的理解了, substr(start, end - start);
    //                                  这里的 start 表示子字符串的起始位置
    //                                        end  表示子字符串结束后的下一个位置
    // 自己有个误解, 以为 substr 传入的 size 是 0 时,也是有一个
    //    实际上传入0时,就什么都没有了
    // ! 这里自己还有一个点想错了, 到这里时, 不是 cur_ 代表引号, cur_ 代表的是引号的下一个了,
    // !         cur_ - 1 才是代表的引号
    // std::cout << "add string:"
    // << source_.substr(start_ + 1, cur_ - 1 - (start_ + 1)) << "\n";
    // 同样的问题
    add_token(
        TokenType::STRING,
        static_cast<std::string_view>(source_).substr(start_ + 1,
                                                      cur_ - 1 - (start_ + 1)));
  }

  void
  number()
  {
    // 自己最终的想法是先获取整数,然后判断小数是否为0,如果为零,就在最后加个.0, 否则什么都不加
    while(is_digit(peek()))
    {
      advance();
    }
    if(peek() == '.' && is_digit(peek_next()))
    {
      advance();
      while(is_digit(peek()))
      {
        advance();
      }
    }
    // cur_ 已经代表的是数字结束后的位置了
    add_token(TokenType::NUMBER,
              std::stod(source_.substr(start_, cur_ - start_)));
    // add_token(TokenType::NUMBER, 2.0);
    // add_token(
    //     TokenType::NUMBER,
    //     static_cast<std::string_view>(source_).substr(start_, cur_ - start_));
  }

  void
  identifier()
  {
    // 这里自己的第一想法还是要先找string...
    // 调用 string 函数, 实际上并没有什么必要...今天睡觉吧
    while(is_alpha_digit(peek()))
    {
      advance();
    }
    // !没有考虑到保留字
    // add_token(TokenType::IDENTIFIER, source_.substr(start_, cur_ - start_));
    // auto iden = source_.substr(start_, cur_ - start_);
    // 这里可以节省一次拷贝
    auto iden = std::string_view(source_).substr(start_, cur_ - start_);
    TokenType type{TokenType::IDENTIFIER};
    if(iden == "class")
    {
      type = TokenType::CLASS;
    }
    else if(iden == "and")
    {
      type = TokenType::AND;
    }
    else if(iden == "else")
    {
      type = TokenType::ELSE;
    }
    else if(iden == "false")
    {
      type = TokenType::FALSE;
    }
    else if(iden == "fun")
    {
      type = TokenType::FUN;
    }
    else if(iden == "for")
    {
      type = TokenType::FOR;
    }
    else if(iden == "if")
    {
      type = TokenType::IF;
    }
    else if(iden == "nil")
    {
      type = TokenType::NIL;
    }
    else if(iden == "or")
    {
      type = TokenType::OR;
    }
    else if(iden == "print")
    {
      type = TokenType::PRINT;
    }
    else if(iden == "return")
    {
      type = TokenType::RETURN;
    }
    else if(iden == "super")
    {
      type = TokenType::SUPER;
    }
    else if(iden == "this")
    {
      type = TokenType::THIS;
    }
    else if(iden == "true")
    {
      type = TokenType::TRUE;
    }
    else if(iden == "var")
    {
      type = TokenType::VAR;
    }
    else if(iden == "while")
    {
      type = TokenType::WHILE;
    }
    else
    {
      type = TokenType::IDENTIFIER;
      // std::cout << "literal:" << iden << "\n";
    }

    add_token(type);
  }

  // 当匹配到时, 需要读取
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
  char
  advance()
  {
    return source_[cur_++];
  }
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

  std::string source_;
  unsigned long int line_{1};
  // 存储的是当次扫描开始的位置
  unsigned long int start_{0};
  // 表示的是当前改扫描的位置(实际上还没有扫描)
  unsigned long int cur_{0};
  std::string_view contents_;
  std::vector<Token> tokens_;
};
} // namespace beacon_lox