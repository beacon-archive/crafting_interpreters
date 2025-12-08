#include "utils/logger.hh"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/pattern_formatter.h>

namespace
{
class FourCharLevelFlag : public spdlog::custom_flag_formatter
{
public:
  void
  format(spdlog::details::log_msg const& msg,
         std::tm const&,
         spdlog::memory_buf_t& dest) override
  {
    std::string_view level_name;
    switch(msg.level)
    {
      case spdlog::level::trace:
        level_name = "TRAC";
        break;
      case spdlog::level::debug:
        level_name = "DEBG";
        break;
      case spdlog::level::info:
        level_name = "INFO";
        break;
      case spdlog::level::warn:
        level_name = "WARN";
        break;
      case spdlog::level::err:
        level_name = "ERRO";
        break;
      case spdlog::level::critical:
        level_name = "CRIT";
        break;
      case spdlog::level::off:
        level_name = "OFF ";
        break;
      default:
        level_name = "UNKN";
        break;
    }
    // 将字符串追加到目标 buffer 中
    dest.append(level_name.data(), level_name.data() + level_name.size());
  }

  [[nodiscard]] std::unique_ptr<custom_flag_formatter>
  clone() const override
  {
    return spdlog::details::make_unique<FourCharLevelFlag>();
  }
};
} // namespace

void
init_log(std::string const& logfile)
{
  auto console_sink_formatter = std::make_unique<spdlog::pattern_formatter>();
  console_sink_formatter->add_flag<FourCharLevelFlag>('l');
  console_sink_formatter->set_pattern(
      "[%^%-4l%$] [%Y-%m-%d %H:%M:%S,%e] [%s:%#] %v");

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::trace); // 控制台显示 debug 及以上

  console_sink->set_formatter(std::move(console_sink_formatter));
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      logfile,          // 日志文件名
      1024 * 1024 * 10, // 10 MB
      3                 // 保留 3 个滚动文件
  );
  file_sink->set_level(spdlog::level::trace); // 文件中记录所有 trace 信息
  auto console_sinkconsole_sink = std::make_unique<spdlog::pattern_formatter>();
  console_sinkconsole_sink->add_flag<FourCharLevelFlag>('l');
  console_sinkconsole_sink->set_pattern(
      "[%^%-4l%$] [%Y-%m-%d %H:%M:%S,%e] [%s:%#] %v");
  file_sink->set_formatter(std::move(console_sinkconsole_sink));
  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
  auto logger =
      std::make_shared<spdlog::logger>("ci_logger", sinks.begin(), sinks.end());

  logger->set_level(spdlog::level::trace);
  spdlog::register_logger(logger);
  spdlog::set_default_logger(logger); // 可选：让 spdlog::info() 也指向它
  spdlog::flush_on(spdlog::level::warn); // 遇到 warn 级别自动 flush
}
