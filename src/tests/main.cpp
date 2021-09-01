#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <fstream>

#include <scene/scene.h>
#include <scene/scene_parser.h>
#include <core/logging.h>

/*
 * #include "something.luisa"
 *
 * Camera camera : Pinhole {
 *   fov {}
 * }
 *
 * Shape shape : Mesh {
 *   file {}
 * }
 *
 * render {
 *
 * }
 *
 */




using namespace gr;

int main() {
    SceneParser parser;
    auto scene = parser.parse("data/test.luisa");
    GR_INFO("Done.");
    scene->print(std::cout);
}
