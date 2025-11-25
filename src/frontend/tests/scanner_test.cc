#include "frontend/scanner.hh"
#include "utils/logger.hh"
#include <sstream>
#include <fstream>
#include <format>
#include <cerrno>
#include <string>
#include <iostream>


// 假设 Scanner 在这个命名空间
// #include "scanner.h"

// 1. 核心执行逻辑：无论是文件还是 REPL，最终都调用这个
void
run(std::string_view source)
{
  beacon_lox::Scanner scanner{std::string(source)};
  auto tokens = scanner.scan_tokens();

  for(const auto token : tokens)
  {
    SPDLOG_DEBUG("{}",
                 std::format("{} {} {}",
                             token.get_type(),
                             token.get_lexeme(),
                             token.get_literal()));
  }

  // 这里可以添加错误检查 logic，如果在扫描中出错，设置一个全局 flag
}

// 2. 文件模式 (类似 python script.py)
void
run_file(const char *path)
{
  std::ifstream file(path);
  if(!file.is_open())
  {
    // 66 (EX_NOINPUT) 是标准的 "无法打开输入" 错误码，或者保持你用的 65
    SPDLOG_ERROR("Could not open file \"{}\". Error: {}",
                 path,
                 strerror(errno));
    exit(74); // IO Error 通常用 74，或者保持 exit(65)
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  // 执行文件内容
  run(buffer.str());

  // 如果在编译/执行文件过程中出错，通常应该以非零状态码退出
  // if (hadError) exit(65);
}

// 3. REPL 模式 (类似直接运行 python)
void
run_prompt()
{
  std::string line;
  while(true)
  {
    std::cout << "> ";

    // 读取一行输入, 自己之前是不知道如何读取一行的输入...，想到了 cin，但是只能读取一个以空格区分的字符串
    if(!std::getline(std::cin, line))
    {
      // 检测到 EOF (Ctrl+D on Linux/Mac, Ctrl+Z on Windows)
      /*
      ➜  /workspace/ci/build/src/frontend/tests git:(main) ✗ /workspace/ci/build/src/frontend/tests/scanner_test
      > [info ] [2025-11-25 18:34:13,866] [scanner_test.cc:114] Bye!
      ➜  /workspace/ci/build/src/frontend/tests git:(main) ✗ /workspace/ci/build/src/frontend/tests/scanner_test
      > [info ] [2025-11-25 18:34:29,793] [scanner_test.cc:114] 
      Bye!
      */
      // 上面是自己犯的臆想的错！
      // SPDLOG_INFO("\nBye!");
      std::cout << "\n";
      SPDLOG_INFO("Bye!");
      break;
    }

    // 执行这一行
    run(line);

    // REPL 模式下，如果出错不应该退出程序，而是重置错误标志
    // hadError = false;
  }
}

int
main(int argc, char **argv)
{
  init_log(); // 你的日志初始化

  if(argc == 1)
  {
    // 没有任何参数 -> 进入交互模式
    run_prompt();
  }
  else if(argc == 2)
  {
    // 一个参数 -> 运行脚本文件
    run_file(argv[1]);
  }
  else
  {
    // 参数过多 -> 打印用法并退出
    // 64 (EX_USAGE) 是标准的用法错误码
    SPDLOG_WARN("Usage: beacon_lox [script]");
    return 64;
  }

  return 0;
}