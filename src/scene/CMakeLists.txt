set(GRENDER_SCENE_SOURCES
        scene_node.cpp scene_node.h
        scene_graph.cpp scene_graph.h
        scene_parser.cpp scene_parser.h)

add_library(grender-scene STATIC ${GRENDER_SCENE_SOURCES})
target_link_libraries(grender-scene PUBLIC grender-core)
