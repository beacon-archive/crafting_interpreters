# src/interpreter/CMakeLists.txt

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Debug)

add_library(interpreter_lib SHARED 
  src/interpreter.cc
  src/lox_object.cc
  src/environment.cc
)

target_include_directories(interpreter_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/src/frontend/include
)
# 如果 frontend 生成了一个库（例如 libfrontend），需要链接它
target_link_libraries(interpreter_lib PUBLIC error logger_lib frontend_lib)


add_executable(interpreter_test tests/interpreter_test.cc)
set(executables
  interpreter_test
)

foreach(execu  IN ITEMS ${executables})
  target_include_directories(${execu} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/src/frontend/include
  )
  # 如果 frontend 生成了一个库（例如 libfrontend），需要链接它
  target_link_libraries(${execu} PUBLIC interpreter_lib)
endforeach()

