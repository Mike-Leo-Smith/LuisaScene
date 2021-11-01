//
// Created by Mike Smith on 2021/9/1.
//

#include <fstream>

#include <core/logging.h>
#include <scene/scene_graph.h>
#include <scene/scene_node.h>
#include <scene/scene_parser.h>

namespace gr {

SceneGraph::SceneGraph(const std::filesystem::path &base_folder) noexcept
    : _base_folder{std::filesystem::canonical(base_folder)} {}

SceneGraph::~SceneGraph() noexcept = default;

void SceneGraph::_add_global_node(std::unique_ptr<SceneNode> node) noexcept {
    auto p_node = node.get();
    if (!_global_nodes.emplace(std::move(node)).second) [[unlikely]] {
        GR_WARNING_WITH_LOCATION(
            "Global node '{}' is overwritten "
            "with new definition.",
            p_node->name());
    }
}

void SceneGraph::_set_root_node(std::unique_ptr<SceneNode> node) noexcept {
    if (_root_node != nullptr) [[unlikely]] {
        GR_WARNING_WITH_LOCATION(
            "Root node is overwritten "
            "with new definition.");
    }
    _root_node = std::move(node);
}

const SceneNode *SceneGraph::_global_node(std::string_view name) noexcept {
    if (auto iter = _global_nodes.find(name);
        iter != _global_nodes.cend()) {
        return iter->get();
    }
    GR_ERROR_WITH_LOCATION(
        "Unresolved reference to global node '{}'.",
        name);
}

void SceneGraph::print(std::ostream &os) const noexcept {
    // TODO: topologically print...
    for (auto &&node : _global_nodes) {
        os << node->base_type_identifier() << " ";
        node->print(os, 0u);
        os << "\n";
    }
    os << root_node_name;
    _root_node->print(os);
}

std::unique_ptr<SceneGraph> SceneGraph::load(const std::filesystem::path &path) noexcept {
    return SceneParser{}.parse(path);
}

void SceneGraph::dump(const std::filesystem::path &path) const noexcept {
    std::ofstream file{path};
    print(file);
}

}// namespace gr
