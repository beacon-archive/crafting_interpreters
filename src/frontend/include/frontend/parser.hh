#pragma once
#include <vector>

#include "error.hh"
#include "ast.hh"
#include "token.hh"

#include "utils/logger.hh"


namespace beacon_lox
{
//////////////////////////
/// @brief 将词法标记(Token)再次转换为更丰富、更复杂的表示形式(表达式:Expression)。
///
/// @date 2025-11-19
//////////////////////////
class Parser
{
public:
  Parser(std::vector<Token> token)
    : tokens_(std::move(token))
  // , cur_iter_(tokens_.begin())
  // , end_iter_(tokens_.end())
  {}

  //////////////////////////
  /// @brief 目前的情况是，一次只能处理一个表达式
  ///
  /// @return Expr
  /// @date 2025-11-20
  //////////////////////////
  auto
  parse() -> Expr
  {
    return expression();
  }

  auto
  parse_stmt() -> StmtList
  {
    StmtList statements;
    while(!is_at_end())
    {
      // statements.push_back(declaration());
      if(auto decl = declaration(); decl.has_value())
      {
        statements.push_back(std::move(decl.value()));
      }
    }

    return statements;
  }

private:
  // 之所以在 declaration 这里加入同步错误,是为了在声明出现错误时,可以顺利的跳转到下一个语句或声明处
  // 今天遇到一个问题,当有语法错误时, 比如语句后面忘记了加 ;
  // 则会导致这里返回一个 nullptr, 引起段错误
  // 这里应该选用 optional
  auto
  declaration() -> std::optional<Stmt>
  {
    try
    {
      if(match(TokenType::VAR))
      {
        return {};
      }

      return std::make_optional<Stmt>(statement());
    }
    catch(std::exception const& e)
    {
      synchronize();
      SPDLOG_ERROR(e.what());
      return std::nullopt;
    }
  }

  auto
  var_declaration() -> Stmt
  {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    Expr initializer;
    if(match(TokenType::EQUAL))
    {
      initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<VarStmt>(name, std::move(initializer));
  }

  auto
  statement() -> Stmt
  {
    // 这里 PRINT 也是一个 token,自己一开始想着是不是该去匹配字符串...
    // 自己还是有些没有转变过来...在解析器中,所有的的都是token!!!!
    if(match(TokenType::PRINT))
    {
      return print_statement();
    }
    return expression_statement();
  }

  auto
  print_statement() -> Stmt
  {
    Expr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");

    return std::make_shared<PrintStmt>(std::move(value));
  }

  auto
  expression_statement() -> Stmt
  {
    Expr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(std::move(expr));
  }


  auto
  expression() -> Expr
  {
    return equality();
  }

  // 对于 1 == 2 == 3 == 4
  // exp: (EQUAL_EQUAL == (EQUAL_EQUAL == (EQUAL_EQUAL == (1.0) (2.0)) (3.0)) (4.0))
  auto
  equality() -> Expr
  {
    // 对于 1 == 2 == 3 来说
    // 这里自己最大的问题点是 以为这个 expr 只是1
    // 然后到了下面的while循环,才得到 1 == 2, 实际上到了这里时, 已经是 1 == 2 了.
    // error 不对,这里思考的不对....!

    // 实际上不是这里自己思考错了,而是后面的自己思考错了,
    // Expr right = comparsion(); 对于这个表达式,自己以为得到的就是 2 == 3 了,
    // 但是实际上它得到的只是一个 2, 而非 2 == 3!
    // 第一次去匹配时!
    // good!!!!!
    Expr expr = comparsion();
    // 满足左结合性
    // 这里自己今天突然觉得应该是右结合性了...
    // 因为自己想到了一个莫名其妙的情况 a == (b == c)
    //                                 这里是一个 comparsion
    //                                 这里自己加了一个括号是因为,这里是一个非终止表达式,会将结果计算完再贴到这里
    //     这里是自己又想多了,或者说没有专注于当前任务,这里只是表示这个表达式,并没有涉及求值...
    //  exp: (EQUAL_EQUAL == (EQUAL_EQUAL == (1.0) (2.0)) (3.0))
    //  虽然表达式的结果和自己预想的一样,但是最终求值的方式和值并不是按照这样来的!
    //!  不,是自己没有理解到这个 exp, 最外层的是根节点, 它有两个字数,左子树是  (EQUAL_EQUAL == (1.0) (2.0)) 右子树是 (3.0)
    //! 自己的问题是将, (EQUAL_EQUAL == (EQUAL_EQUAL ==  这里的顺序和 1 == 2 == 3 对应上了!!
    //!                        自己没有意y到 exp 中的 == 顺序和表达式中的 == 顺序是不同的!
    while(match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
    {
      // Token operator = p
      // 这个获取的是运算符,而非之前的那个表达式
      auto token = previos();
      Expr right = comparsion();
      // 只有这里构造的 expr 才是包含有 == 或者 != 的, 所以这里一定是左结合性
      expr = std::make_unique<BinaryExpr>(std::move(expr),
                                          token,
                                          std::move(right));
    }

    return expr;
  }

  auto
  comparsion() -> Expr
  {
    Expr exp = term();
    while(match(TokenType::GREATER,
                TokenType::GREATER_EQUAL,
                TokenType::LESS,
                TokenType::LESS_EQUAL))
    {
      auto token = previos();
      Expr right = term();
      exp =
          std::make_unique<BinaryExpr>(std::move(exp), token, std::move(right));
    }
    return exp;
  }

  auto
  term() -> Expr
  {
    Expr exp = factor();
    while(match(TokenType::PLUS, TokenType::MINUS))
    {
      auto token = previos();
      Expr right = factor();
      exp =
          std::make_unique<BinaryExpr>(std::move(exp), token, std::move(right));
    }
    return exp;
  }

  auto
  factor() -> Expr
  {
    Expr exp = unary();
    while(match(TokenType::STAR, TokenType::SLASH))
    {
      auto token = previos();
      Expr right = unary();
      exp =
          std::make_unique<BinaryExpr>(std::move(exp), token, std::move(right));
    }
    return exp;
  }

  auto
  unary() -> Expr
  {
    if(match(TokenType::BANS, TokenType::MINUS))
    {
      auto token = previos();
      auto expr = primary();
      Expr exp = std::make_unique<UnaryExpr>(token, std::move(expr));
      return exp;
    }

    return primary();
  }

  auto
  primary() -> Expr
  {
    if(match(TokenType::FALSE))
    {
      return std::make_unique<LiteralExpr>(false);
    }
    if(match(TokenType::TRUE))
    {
      return std::make_unique<LiteralExpr>(true);
    }
    if(match(TokenType::NIL))
    {
      return std::make_unique<LiteralExpr>(nullptr);
    }
    if(match(TokenType::NUMBER, TokenType::STRING))
    {
      auto token = previos();
      return std::make_unique<LiteralExpr>(token.get_literal());
    }
    if(match(TokenType::IDENTIFIER))
    {
      return std::make_unique<VarExpr>(previos());
    }
    if(match(TokenType::LEFT_PAREN))
    {
      Expr exp = expression();
      consume(TokenType::RIGHT_PAREN, "( not match!");
      return std::make_unique<GroupingExpr>(std::move(exp));
    }

    throw std::runtime_error("Unexpect expression.");
  }

  auto
  consume(TokenType const& type, std::string_view sv) -> Token
  {
    if(check(type))
    {
      return advance();
    }
    g_error.error(error(peek(), sv));

    throw error(peek(), "Unexpect type");
  }


  auto
  error(Token token, std::string_view sv) -> Error::RuntimeError
  {
    throw Error::RuntimeError(token, std::string(sv));
  }

  class ParseError : public std::runtime_error
  {
  public:
    ParseError(std::string const& message = "Parse error occurred")
      : std::runtime_error(message)
    {}
  };

  auto
  synchronize() -> void
  {
    advance();
    while(!is_at_end())
    {
      if(previos().get_type() == TokenType::SEMICOLON)
      {
        return;
      }

      switch(previos().get_type())
      {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::PRINT:
        case TokenType::WHILE:
        case TokenType::RETURN:
          return;
        default:
          break;
      }
      advance();
    }
  }

  // 关于 match 自己的意图和作者的存在区别
  // 作者书中的意图是当发现匹配时,就会消耗掉这个token
  // [[nodiscard]] auto
  // match(const TokenType &type) const -> bool
  // {
  //   if(is_at_end())
  //   {
  //     return false;
  //   }

  //   return type == tokens_[cur_].get_type();
  // }
  //* 可以合并到一个模板参数中
  // [[nodiscard]] auto
  // match(const TokenType &type) -> bool
  // {
  //   if(check(type))
  //   {
  //     advance();
  //     return true;
  //   }
  //   return false;
  // }

  // [[nodiscard]] auto
  // check(const TokenType &type) const -> bool
  // {
  //   if(is_at_end())
  //   {
  //     return false;
  //   }

  //   return type == tokens_[cur_].get_type();
  // }
  template <typename... Args>
  [[nodiscard]] auto
  check(Args const&... args) const -> bool
  {
    if(is_at_end())
    {
      return false;
    }

    // 这种空参数包是自己没有想到的, gpt 进行了补充
    if constexpr(sizeof...(args) == 0)
    {
      return false; // 空参数包
    }
    return ((tokens_[cur_].get_type() == args) || ...);
  }

  template <typename... Args>
  [[nodiscard]] auto
  match(Args... args) -> bool
  {
    // 添加静态断言, 确保传入的参数类型符合预期
    static_assert((std::is_same_v<std::decay_t<Args>, TokenType> && ...),
                  "All arguments must be of type TokenType");

    if(check(args...))
    {
      advance();
      return true;
    }

    return false;
  }

  [[nodiscard]] auto
  peek() -> Token const&
  {
    // 感觉这里应该还是要加一个处理的
    //   if(cur_iter_ == end_iter_)
    //   {
    //     return
    //   }
    return tokens_[cur_];
  }

  [[nodiscard]] auto
  previos() -> Token const&
  {
    return tokens_[cur_ - 1];
  }

  [[nodiscard]] auto
  is_at_end() const -> bool
  {
    return tokens_[cur_].get_type() == TokenType::LOX_EOF;
  }

  // 昨天自己有个疑问, 如果超过最后一个字符要怎么处理,这里显示了结果
  // 如果已经到了最后了, 不移动 迭代器 就可以了
  Token
  advance()
  {
    if(!is_at_end())
    {
      ++cur_;
    }
    return previos();
  }

  std::vector<Token> tokens_;
  // std::vector<Token>::iterator cur_iter_;
  int cur_{0};
  // 预期: 每个 token 序列的最后一个都是 EOF!
  // 所以这里没有必要单独存储一个 end 了
  // std::vector<Token>::iterator end_iter_;
};
} // namespace beacon_lox