#include "frontend/parser.hh"
#include "frontend/scanner.hh"

#include "utils/logger.hh"

#include <iostream>
#include <fstream>
#include <sstream>


int
main(int /*argc*/, char ** /*argv*/)
{
  init_log();
  // std::string_view path{argv[1]};
  std::stringstream ss;
  std::string file_path = "/workspace/ci/data/bea.lox";
  std::fstream file(file_path);
  if(!file.is_open())
  {
    SPDLOG_ERROR("failed open file {}, error: {}",
                 file_path,
                 std::strerror(errno));
    return 65;
  }
  ss << file.rdbuf();
  beacon_lox::Scanner scanner{ss.str()};
  auto tokens = scanner.scan_tokens();

  for(const auto token : tokens)
  {
    SPDLOG_DEBUG("tokens: {}",
                 std::format("{} {} {}",
                             token.get_type(),
                             token.get_lexeme(),
                             token.get_literal()));
  }

  beacon_lox::Parser par(tokens);
  try
  {
    auto expr = par.parse();
    SPDLOG_INFO("-------------------------");

    beacon_lox::ExprVisitor visitor;
    SPDLOG_DEBUG("exp: {}",
                 std::any_cast<std::string>(std::visit(
                     [&visitor](const auto &value) -> std::any
                     {
                       using T = std::decay_t<decltype(value)>;
                       if constexpr(std::is_same_v<T, std::monostate>)
                       {
                         return {"nil"}; // 或者处理空指针逻辑
                       }
                       else
                       {
                         return value->accept(&visitor);
                       }
                     },
                     expr)));
  }
  catch(const std::exception &e)
  {
    SPDLOG_ERROR("exception: {}", e.what());
  }


  return 0;
}