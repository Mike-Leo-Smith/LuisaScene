set(GRENDER_CORE_SOURCES
        logging.cpp logging.h file_reader.cpp file_reader.h string_hash.h)

add_library(grender-core STATIC ${GRENDER_CORE_SOURCES})
target_link_libraries(grender-core PUBLIC grender-ext grender-include)
