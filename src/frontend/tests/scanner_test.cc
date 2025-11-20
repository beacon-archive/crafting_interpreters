#include "frontend/scanner.hh"
#include "utils/logger.hh"
#include <sstream>
#include <fstream>
#include <format>
#include <cerrno>
#include <string>

int
main(int /*argc*/, char ** /*argv*/)
{
  init_log();
  SPDLOG_INFO("Hello INFO");
  SPDLOG_DEBUG("Hello DEBUG");
  SPDLOG_TRACE("Hello TRACE");
  // std::string_view path{argv[1]};
  std::stringstream ss;
  std::string file_path("/workspace/ci/data/bea.lox");
  std::fstream file(file_path);
  if(!file.is_open())
  {
    SPDLOG_ERROR("open {} failed! err: {}", file_path, strerror(errno));
    return 65;
  }
  ss << file.rdbuf();
  beacon_lox::Scanner scanner{ss.str()};
  auto tokens = scanner.scan_tokens();
  spdlog::info("SPDLOG_ACTIVE_LEVEL = {}", SPDLOG_ACTIVE_LEVEL);
  spdlog::info("default logger level: {}",
               static_cast<int>(spdlog::default_logger()->level()));

  for(const auto token : tokens)
  {
    SPDLOG_DEBUG("{}",
                 std::format("{} {} {}",
                             token.get_type(),
                             token.get_lexeme(),
                             token.get_literal()));
  }
  return 0;
}
