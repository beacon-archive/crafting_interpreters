#include <sstream>
#include <fstream>

#include "frontend/parser.hh"
#include "frontend/scanner.hh"
#include "interpreter/interpreter.hh"


int
main(int /*argc*/, char ** /*argv*/)
{
  // std::string_view path{argv[1]};
  std::stringstream ss;
  std::fstream file("/workspace/crafting_interpreters/data/bea.lox");
  if(!file.is_open())
  {
    return 65;
  }
  ss << file.rdbuf();
  beacon_lox::Scanner scanner{ss.str()};
  auto tokens = scanner.scan_tokens();

  for(const auto token : tokens)
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
    auto expr = par.parse();
    std::cout << "-------------------------\n";

    beacon_lox::ExprVisitor visitor;

    std::cout << std::format("exp: {}\n",
                             std::any_cast<std::string>(std::visit(
                                 [&visitor](const auto &value) -> std::any
                                 { return value->accept(&visitor); },
                                 expr)));

    std::cout << "-------------------------\n";
    // 这里有个问题, interpreter 本质是一个 visitor, 所以这里不太清楚应该如何在 Parser 中使用
    // std::cout << "expr idx:" << expr.index() << "\n";
    inter.interpret(expr);

    if(inter.had_runtime_error())
    {
      return 70;
    }

    if(inter.had_error())
    {
      return 65;
    }
  }
  catch(const std::exception &e)
  {
    std::cout << "exception: " << e.what() << "\n";
  }


  return 0;
}