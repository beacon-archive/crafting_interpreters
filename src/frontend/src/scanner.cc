#include "frontend/scanner.hh"


namespace beacon_lox
{
void
Scanner::identifier()
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

void
Scanner::number()
{
  // 自己最终的想法是先获取整数,然后判断小数是否为0,如果为零,就在最后加个.0, 否则什么都不加
  while(is_digit(peek()))
  {
    // advance();
    ++cur_;
  }
  if(peek() == '.' && is_digit(peek_next()))
  {
    // 这个消耗的是 .
    // advance();
    ++cur_;
    // 消耗后面的数字
    while(is_digit(peek()))
    {
      // advance();
      ++cur_;
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

//////////////////////////
/// @brief  这里支持多行字符串，有个问题是：禁止换行比允许换行更复杂一些，这是自己没有想到的。
///
/// @date 2025-11-18
//////////////////////////
void
Scanner::string()
{
  while(peek() != '"' && !is_end())
  {
    if(peek() == '\n')
    {
      ++line_;
    }
    // advance();
    ++cur_;
  }

  if(is_end())
  {
    // lex: error
    return;
  }

  // 忘记了后引号
  // advance();
  ++cur_;
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
  // 这里要排除前引号和后引号
  // 字符串才有字面量： static_cast<std::string_view>(source_).substr(start_ + 1, cur_ - 1 - (start_ + 1))
  // 有个问题是，这里使用 string_view 是否存在风险
  add_token(
      TokenType::STRING,
      static_cast<std::string_view>(source_).substr(start_ + 1,
                                                    cur_ - 1 - (start_ + 1)));
}

void
Scanner::scan_token()
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
    // 这里处理的就是 单字符词素， 或者操作符
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
    // \r 是回车
    // 有个问题，会车符，需要增加 line 么？
    case '\r':
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
      // 这里判断的是 保留字(类似于关键字，类名之类的) 和 标识符(应该就是变量名字了)
      else if(is_alpha(c))
      {
        identifier();
      }
      else
      {
        // 错误的字符，目前也会被消费，这样就能避免无限循环了
        SPDLOG_ERROR("Unexpected character: {}", c);
      }
      break;
  }
}

} // namespace beacon_lox