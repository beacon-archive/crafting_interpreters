# src/frontend/tests/CMakeLists.txt
add_executable(scanner_test scanner_test.cc)
add_executable(ast_test ast_print.cc ast_visiter.cc)
add_executable(parser parser_test.cc ast_visiter.cc)


set(executables
  scanner_test
  ast_test
  parser
)

foreach(execu  IN ITEMS ${executables})
  target_include_directories(${execu} PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
  )
  
  target_link_libraries(${execu} PUBLIC frontend_lib error logger_lib)
endforeach()
