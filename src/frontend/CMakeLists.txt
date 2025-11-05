# src/scanner/CMakeLists.txt

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Debug)


add_library(frontend SHARED
   src/scanner.cc
)

target_include_directories(frontend PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
# 确保spdlog可以被frontend使用
target_link_libraries(frontend PUBLIC
    logger_lib
)

# 定义 frontend 库
add_library(error STATIC
    src/error.cc
)
target_include_directories(error PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(test tests/scanner_test.cc)
add_executable(ast tests/ast_print.cc)
add_executable(parser tests/parser_test.cc src/error.cc)


set(executables
  test
  ast
  parser
)

foreach(execu  IN ITEMS ${executables})
  target_include_directories(${execu} PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
endforeach()
