#include <scene/scene_graph.h>

using namespace gr;

int main() {
    auto scene_graph = SceneGraph::load("data/test.grscene");
    scene_graph->dump("data/test-dump.grscene");
}
