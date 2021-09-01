add_library(grender-ext INTERFACE)

set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)
set(SPDLOG_ENABLE_PCH ON CACHE BOOL "" FORCE)
add_subdirectory(spdlog)
set_target_properties(spdlog PROPERTIES UNITY_BUILD ON)
target_compile_definitions(spdlog PUBLIC FMT_CONSTEVAL=inline) # clang has problem with this consteval
target_link_libraries(grender-ext INTERFACE spdlog::spdlog)
