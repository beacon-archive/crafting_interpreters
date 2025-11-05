#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>



class LoggerInitialize
{
public:
  LoggerInitialize()
  {
    spdlog::set_pattern("[%l] [%Y-%m-%d %H:%M:%S,%e] [%s:%#] %v");

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace); // 控制台显示 debug 及以上

    console_sink->set_pattern("[%^%-8l%$] [%Y-%m-%d %H:%M:%S,%e] [%s:%#] %v");
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logs/simulation.log", // 日志文件名
        1024 * 1024 * 10,      // 10 MB
        3                      // 保留 3 个滚动文件
    );
    file_sink->set_level(spdlog::level::trace); // 文件中记录所有 trace 信息
    file_sink->set_pattern("[%^%-8l%$] [%Y-%m-%d %H:%M:%S,%e] [%s:%#] %v");

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("sim_logger",
                                                   sinks.begin(),
                                                   sinks.end());

    logger->set_level(spdlog::level::trace);
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger); // 可选：让 spdlog::info() 也指向它
    spdlog::flush_on(spdlog::level::warn); // 遇到 warn 级别自动 flush
  }
};

static LoggerInitialize const logger_init;