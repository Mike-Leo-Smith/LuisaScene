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

class SceneGraphNode;
class SceneParser;

class SceneGraph {

public:
    struct NodeEqual {
        using is_transparent = void;
        [[nodiscard]] auto operator()(const std::unique_ptr<SceneGraphNode> &lhs, const std::unique_ptr<SceneGraphNode> &rhs) const noexcept {
            return lhs->name() == rhs->name();
        }
        [[nodiscard]] auto operator()(std::string_view lhs, const std::unique_ptr<SceneGraphNode> &rhs) const noexcept {
            return lhs == rhs->name();
        }
        [[nodiscard]] auto operator()(const std::unique_ptr<SceneGraphNode> &lhs, std::string_view rhs) const noexcept {
            return lhs->name() == rhs;
        }
        [[nodiscard]] auto operator()(std::string_view lhs, std::string_view rhs) const noexcept {
            return lhs == rhs;
        }
    };

    struct NodeHash : StringHash {
        using StringHash::operator();
        [[nodiscard]] auto operator()(const std::unique_ptr<SceneGraphNode> &node) const noexcept {
            return StringHash::operator()(node->name());
        }
    };

public:
    static constexpr std::string_view root_node_name = "render";

private:
    std::filesystem::path _base_folder;
    std::unique_ptr<SceneGraphNode> _root_node;// the node with name "render"
    std::unordered_set<std::unique_ptr<SceneGraphNode>, NodeHash, NodeEqual> _global_nodes;

private:
    friend class SceneParser;
    explicit SceneGraph(const std::filesystem::path &base_folder) noexcept;
    void _add_global_node(std::unique_ptr<SceneGraphNode> node) noexcept;
    const SceneGraphNode *_global_node(std::string_view name) noexcept;
    void _set_root_node(std::unique_ptr<SceneGraphNode> node) noexcept;

public:
    ~SceneGraph() noexcept;
    SceneGraph(SceneGraph &&) noexcept = delete;
    SceneGraph(const SceneGraph &) noexcept = delete;
    SceneGraph &operator=(SceneGraph &&) noexcept = delete;
    SceneGraph &operator=(const SceneGraph &) noexcept = delete;
    [[nodiscard]] decltype(auto) base_folder() const noexcept { return (_base_folder); }
    [[nodiscard]] auto root() const noexcept { return const_cast<const SceneGraphNode *>(_root_node.get()); }

    // for debug
    void print(std::ostream &os) const noexcept;

    // load & dump
    [[nodiscard]] static std::unique_ptr<SceneGraph> load(const std::filesystem::path &path) noexcept;
    void dump(const std::filesystem::path &path) const noexcept;
};

}// namespace gr
