#include "frontend/scanner.hh"
#include <sstream>
#include <fstream>
#include <iostream>
#include <format>

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
  return 0;
}