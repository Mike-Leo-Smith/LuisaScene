add_subdirectory(ext)

add_library(grender-include INTERFACE)
target_include_directories(grender-include INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(grender-include INTERFACE /Zc:preprocessor)
endif ()

add_subdirectory(core)
add_subdirectory(scene)
add_subdirectory(tests)
