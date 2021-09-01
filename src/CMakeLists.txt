add_subdirectory(ext)

add_library(grender-include INTERFACE)
target_include_directories(grender-include INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(core)
add_subdirectory(scene)
add_subdirectory(tests)
