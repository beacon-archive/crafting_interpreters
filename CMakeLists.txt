cmake_minimum_required(VERSION 3.20)
project(CraftingInterpreters LANGUAGES CXX)


# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# 调试和编译命令
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Debug)

# 使用 FetchContent 模块来获取外部仓库
include(FetchContent)

# 2. 声明并获取 spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG        v1.16.0 
)
# 这一行会定义 ${gflags_BINARY_DIR} 变量
FetchContent_MakeAvailable(spdlog)


add_subdirectory(src/frontend)
add_subdirectory(src/interpreter)
add_subdirectory(src/utils)