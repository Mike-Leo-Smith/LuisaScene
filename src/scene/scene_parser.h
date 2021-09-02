//
// Created by Mike Smith on 2021/9/1.
//

#pragma once

#include <string>
#include <string_view>

#include <core/file_reader.h>
#include <scene/scene_node.h>
#include "scene_graph.h"

namespace gr {

class SceneGraph;

class SceneParser {

private:
    mutable size_t _cursor{0u};
    mutable std::string _source;
    mutable std::vector<std::filesystem::path> _path_stack;
    mutable std::unique_ptr<SceneGraph> _scene;

public:
    void _preprocess(FileReader file) const noexcept;
    void _parse_main() const noexcept;
    void _match(char c) const noexcept;
    void _skip(size_t n = 1u) const noexcept;
    [[nodiscard]] char _peek() const noexcept;
    [[nodiscard]] char _get() const noexcept;
    [[nodiscard]] bool _eof() const noexcept;
    [[nodiscard]] std::string_view _read_identifier() const noexcept;
    [[nodiscard]] std::string _read_string() const noexcept;
    [[nodiscard]] double _read_number() const noexcept;
    [[nodiscard]] bool _read_bool() const noexcept;
    void _skip_blanks() const noexcept;
    void _parse_node_body(SceneNode &node) const noexcept;
    [[nodiscard]] SceneNode::value_list_variant _parse_value_list() const noexcept;

public:
    SceneParser() noexcept = default;
    [[nodiscard]] std::unique_ptr<SceneGraph> parse(const std::filesystem::path &main_file) const noexcept;
};

}// namespace gr
