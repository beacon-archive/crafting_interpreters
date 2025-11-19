# src/scanner/CMakeLists.txt
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

add_subdirectory(tests)

