# src/scanner/CMakeLists.txt

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Debug)

add_executable(interpreter tests/interpreter_test.cc)

set(executables
  interpreter
)

foreach(execu  IN ITEMS ${executables})
  target_include_directories(interpreter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/frontend/include
  )
  # 如果 frontend 生成了一个库（例如 libfrontend），需要链接它
  target_link_libraries(interpreter PRIVATE error logger_lib)
endforeach()

# # Set include directories for interpreter
# target_include_directories(interpreter PUBLIC
#     ${CMAKE_SOURCE_DIR}/src/frontend/include
#     ${CMAKE_SOURCE_DIR}/src/interpreter/include
# )