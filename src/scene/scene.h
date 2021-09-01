//
// Created by Mike Smith on 2021/9/1.
//

#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_set>

#include <core/string_hash.h>
#include <scene/scene_node.h>

namespace gr {

class SceneNode;
class SceneParser;

class Scene {

public:
    struct NodeEqual {
        using is_transparent = void;
        [[nodiscard]] auto operator()(const std::unique_ptr<SceneNode> &lhs, const std::unique_ptr<SceneNode> &rhs) const noexcept {
            return lhs->name() == rhs->name();
        }
        [[nodiscard]] auto operator()(std::string_view lhs, const std::unique_ptr<SceneNode> &rhs) const noexcept {
            return lhs == rhs->name();
        }
        [[nodiscard]] auto operator()(const std::unique_ptr<SceneNode> &lhs, std::string_view rhs) const noexcept {
            return lhs->name() == rhs;
        }
        [[nodiscard]] auto operator()(std::string_view lhs, std::string_view rhs) const noexcept {
            return lhs == rhs;
        }
    };

    struct NodeHash : StringHash {
        using StringHash::operator();
        [[nodiscard]] auto operator()(const std::unique_ptr<SceneNode> &node) const noexcept {
            return StringHash::operator()(node->name());
        }
    };

public:
    static constexpr std::string_view root_node_name = "render";

private:
    std::filesystem::path _base_folder;
    std::unique_ptr<SceneNode> _root_node;// the node with name "render"
    std::unordered_set<std::unique_ptr<SceneNode>, NodeHash, NodeEqual> _global_nodes;

private:
    friend class SceneParser;
    explicit Scene(const std::filesystem::path &base_folder) noexcept;
    void _add_global_node(std::unique_ptr<SceneNode> node) noexcept;
    const SceneNode *_global_node(std::string_view name) noexcept;
    void _set_root_node(std::unique_ptr<SceneNode> node) noexcept;

public:
    ~Scene() noexcept;
    Scene(Scene &&) noexcept = delete;
    Scene(const Scene &) noexcept = delete;
    Scene &operator=(Scene &&) noexcept = delete;
    Scene &operator=(const Scene &) noexcept = delete;
    [[nodiscard]] decltype(auto) base_folder() const noexcept { return (_base_folder); }
    [[nodiscard]] auto root() const noexcept { return const_cast<const SceneNode *>(_root_node.get()); }

    // for debug
    void print(std::ostream &os) const noexcept;
};

}// namespace gr
