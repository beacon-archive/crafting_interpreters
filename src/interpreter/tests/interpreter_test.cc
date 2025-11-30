#include <sstream>
#include <fstream>

#include "frontend/parser.hh"
#include "frontend/scanner.hh"
#include "interpreter/interpreter.hh"


int
main(int /*argc*/, char** /*argv*/)
{
  init_log();
  // std::string_view path{argv[1]};
  std::stringstream ss;
  std::fstream file("/workspace/crafting_interpreters/bea.lox");
  if(!file.is_open())
  {
    return 65;
  }
  ss << file.rdbuf();
  beacon_lox::Scanner scanner{ss.str()};
  auto tokens = scanner.scan_tokens();

  for(auto const token : tokens)
  {
    // std::cout << "format:" << token.get_lexeme() << "\n";
    std::cout << std::format("{} {} {}\n",
                             token.get_type(),
                             token.get_lexeme(),
                             token.get_literal());
  }


  beacon_lox::Parser par(tokens);
  beacon_lox::Interpreter inter;
  try
  {
    auto expr = par.parse_stmt();
    std::cout << "-------------------------\n";

    beacon_lox::ExprVisitor visitor;

    // std::cout << std::format(
    //     "exp: {}\n",
    //     std::any_cast<std::string>(std::visit(
    //         [&visitor](auto const& value) -> std::any
    //         {
    //           using T = std::decay_t<decltype(value)>;
    //           if constexpr(std::is_same_v<T, std::monostate>)
    //           {
    //             return {"nil"}; // 或者处理空指针逻辑
    //           }
    //           else
    //           {
    //             return value->accept(&visitor);
    //           }
    //         },
    //         expr)));

    std::cout << "-------------------------\n";
    // 这里有个问题, interpreter 本质是一个 visitor, 所以这里不太清楚应该如何在 Parser 中使用
    // std::cout << "expr idx:" << expr.index() << "\n";
    inter.interpret_stmt(expr);

    if(inter.had_runtime_error())
    {
      return 70;
    }

    if(inter.had_error())
    {
      return 65;
    }
  }
  catch(std::exception const& e)
  {
    std::cout << "exception: " << e.what() << "\n";
  }
  return 0;
}