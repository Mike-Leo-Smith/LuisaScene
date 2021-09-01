//
// Created by Mike Smith on 2021/9/1.
//

#include <core/logging.h>
#include <scene/scene.h>
#include <scene/scene_node.h>

namespace gr {

Scene::Scene(const std::filesystem::path &base_folder) noexcept
    : _base_folder{std::filesystem::canonical(base_folder)} {}

Scene::~Scene() noexcept = default;

void Scene::_add_global_node(std::unique_ptr<SceneNode> node) noexcept {
    auto p_node = node.get();
    if (!_global_nodes.emplace(std::move(node)).second) [[unlikely]] {
        GR_WARNING_WITH_LOCATION(
            "Global node '{}' is overwritten "
            "with new definition.",
            p_node->name());
    }
}

void Scene::_set_root_node(std::unique_ptr<SceneNode> node) noexcept {
    if (_root_node != nullptr) [[unlikely]] {
        GR_WARNING_WITH_LOCATION(
            "Root node is overwritten "
            "with new definition.");
    }
    _root_node = std::move(node);
}

const SceneNode *Scene::_global_node(std::string_view name) noexcept {
    if (auto iter = _global_nodes.find(name);
        iter != _global_nodes.cend()) {
        return iter->get();
    }
    GR_ERROR_WITH_LOCATION(
        "Unresolved reference to global node '{}'.",
        name);
}

void Scene::print(std::ostream &os) const noexcept {
    for (auto &&node : _global_nodes) {
        os << node->base_type_identifier() << " ";
        node->print(os, 0u);
        os << "\n";
    }
    os << "render";
    _root_node->print(os);
}

}// namespace gr
