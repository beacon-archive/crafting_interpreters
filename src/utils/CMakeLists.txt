# 确保spdlog已经被下载和构建
include(FetchContent)

# 如果spdlog还没有被声明，在这里声明
if(NOT spdlog_POPULATED)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG        v1.16.0
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# 创建logger库
add_library(logger_lib
    src/logger.cc
)

# 添加包含目录
target_include_directories(logger_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${spdlog_SOURCE_DIR}/include
)

target_compile_definitions(logger_lib PRIVATE "SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
target_precompile_headers(logger_lib
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/logger.hh
)


# 链接spdlog库
target_link_libraries(logger_lib PRIVATE spdlog::spdlog)


# 如果需要，可以安装这个库
# install(TARGETS logger_lib
#     EXPORT loggerTargets
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
# )
# install(DIRECTORY include/ DESTINATION include)