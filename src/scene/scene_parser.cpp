//
// Created by Mike Smith on 2021/9/1.
//

#include <filesystem>
#include <string_view>

#include <core/logging.h>
#include <scene/scene_graph.h>
#include <scene/scene_parser.h>

namespace gr {

std::unique_ptr<SceneGraph> SceneParser::parse(const std::filesystem::path &main_file) const noexcept {
    _cursor = 0u;
    _source.clear();
    _scene = std::unique_ptr<SceneGraph>{new SceneGraph{main_file.parent_path()}};
    _preprocess(FileReader{main_file});
    _parse_main();
    std::unique_ptr<SceneGraph> scene;
    _scene.swap(scene);
    _cursor = 0u;
    _source.clear();
    return std::move(scene);
}

void SceneParser::_parse_main() const noexcept {

    using namespace std::string_view_literals;
    // GR_INFO("Main file after preprocessing:\n", _source);

    _skip_blanks();
    while (!_eof()) {
        auto identifier = _read_identifier();
        if (identifier == SceneGraph::root_node_name) {// found root node
            auto node = new SceneGraphNode{std::string{SceneGraph::root_node_name}, ""};
            _parse_node_body(*node);
            _scene->_set_root_node(std::unique_ptr<SceneGraphNode>{node});
        } else {
            auto base_type = identifier;
            _skip_blanks();
            auto node_name = _read_identifier();
            _skip_blanks();
            _match(':');
            _skip_blanks();
            auto impl_type = _read_identifier();
            auto node = new SceneGraphNode{std::string{node_name}, std::string{base_type}.append(":").append(impl_type)};
            _parse_node_body(*node);
            _scene->_add_global_node(std::unique_ptr<SceneGraphNode>{node});
        }
        _skip_blanks();
    }
}

void SceneParser::_preprocess(FileReader file) const noexcept {
    auto path_string = file.path().string();
    auto report_error = [&path_string](auto c, auto expected) noexcept {
        GR_ERROR(
            "Unexpected character '", c, "' (expected '", expected, "') ",
            "in preprocess sequence in file '", path_string, "'.");
    };
    {
        std::ostringstream oss;
        oss << "!begin " << file.path() << "\n";
        _source.append(oss.str());
    }
    for (auto c = file.get(); c != EOF; c = file.get()) {
        if (c != '#') {
            _source.push_back(c);
        } else {
            // skip blanks
            while (std::isblank(c = file.get())) {}
            // read "include"
            if (c != 'i') [[unlikely]] { report_error(c, 'i'); }
            using namespace std::string_view_literals;
            for (auto x : "nclude"sv) {
                if ((c = file.get()) != x) [[unlikely]] { report_error(c, x); }
            }
            // skip blanks
            while (std::isblank(c = file.get())) {}
            if (c != '"') [[unlikely]] { report_error(c, '"'); }
            std::string file_name;
            while ((c = file.get()) != '"') {
                constexpr auto s = "!@#$%^&*()-_+=|~`{}[];<>,.?/ "sv;
                if (!isalnum(c) && s.find(c) == std::string_view::npos) [[unlikely]] {
                    GR_ERROR_WITH_LOCATION(
                        "Unexpected character '", c, "' in file ", file.path(), ".");
                }
                file_name.push_back(c);
            }
            std::filesystem::path file_path{file_name};
            if (file_path.is_relative()) { file_path = file.path().parent_path() / file_path; }
            _preprocess(FileReader{file_path});
        }
    }
    std::stringstream oss;
    oss.clear();
    oss << "\n!end " << file.path() << "\n";
    _source.append(oss.str());
}

inline void SceneParser::_skip_blanks() const noexcept {
    auto skip_blanks_exactly = [this] {
        auto check = [this] {
            auto c = _peek();
            return isblank(c) || c == '\r' || c == '\n';
        };
        while (!_eof() && check()) { _skip(); }
    };
    skip_blanks_exactly();
    if (!_eof()) {
        if (_peek() == '!') {
            _skip();
            skip_blanks_exactly();
            using namespace std::string_view_literals;
            if (auto directive = _read_identifier(); directive == "begin"sv) {
                skip_blanks_exactly();
                auto s = _read_string();
                auto path = std::filesystem::canonical(s).parent_path();
                _path_stack.emplace_back(std::move(path));
            } else if (directive == "end"sv) {
                skip_blanks_exactly();
                static_cast<void>(_read_string());// for performance, we do not check it
                _path_stack.pop_back();
            } else {
                GR_ERROR_WITH_LOCATION(
                    "Unexpected meta-data directive '", directive, "'.");
            }
            _skip_blanks();
        } else if (_peek() == '/') {
            _skip();
            _match('/');
            auto eol = [this] {
                auto c = _peek();
                return c == '\n' || c == '\r';
            };
            while (!_eof() && !eol()) { _skip(); }
            _skip_blanks();
        }
    }
}

inline void SceneParser::_match(char c) const noexcept {
    if (auto real_c = _get(); real_c != c) {
        GR_ERROR_WITH_LOCATION(
            "Unexpected character '", real_c, "' (expected ", c, ").");
    }
}

inline char SceneParser::_peek() const noexcept {
    if (_eof()) {
        GR_ERROR_WITH_LOCATION(
            "Failed to peek character at the end of file.");
    }
    return _source[_cursor];
}

inline char SceneParser::_get() const noexcept {
    if (_eof()) {
        GR_ERROR_WITH_LOCATION(
            "Failed to get character at the end of file.");
    }
    return _source[_cursor++];
}

inline void SceneParser::_skip(size_t n) const noexcept {
    _cursor = std::min(_cursor + n, _source.size());
}

inline bool SceneParser::_eof() const noexcept {
    return _cursor >= _source.size();
}

inline std::string_view SceneParser::_read_identifier() const noexcept {
    auto check = [this] {
        auto c = _peek();
        return isalnum(c) || c == '-' || c == '_';
    };
    auto p = _cursor;
    while (check()) { _skip(); }
    return std::string_view{_source}.substr(p, _cursor - p);
}

inline std::string SceneParser::_read_string() const noexcept {
    _match('"');
    std::string s;
    auto c = _get();
    while (c != '"') {
        if (c == '\n' || c == '\r') [[unlikely]] {
            GR_ERROR_WITH_LOCATION("Unexpected new line in string.");
        }
        if (c == '\\') { c = _get(); }
        s.push_back(c);
        c = _get();
    }
    return s;
}

inline SceneGraphNode::value_list_variant SceneParser::_parse_value_list() const noexcept {
    _skip_blanks();
    _match('{');
    _skip_blanks();
    if (auto c = _peek(); isdigit(c) || c == '.' || c == '-') {// numbers
        std::vector<double> numbers{_read_number()};
        _skip_blanks();
        while (_peek() == ',') {
            _skip();
            _skip_blanks();
            numbers.emplace_back(_read_number());
            _skip_blanks();
        }
        _match('}');
        return numbers;
    } else if (c == '@') {// references to global nodes
        _skip();
        std::vector<const SceneGraphNode *> nodes{_scene->_global_node(_read_identifier())};
        _skip_blanks();
        while (_peek() == ',') {
            _skip();
            _skip_blanks();
            _match('@');
            _skip_blanks();
            nodes.emplace_back(_scene->_global_node(_read_identifier()));
            _skip_blanks();
        }
        _match('}');
        return nodes;
    } else if (c == '"') {// paths
        auto read_canonical_path = [this] {
            _skip_blanks();
            std::filesystem::path s{_read_string()};
            if (s.is_relative()) { s = _path_stack.back() / s; }
            return std::filesystem::canonical(s);
        };
        std::vector<std::filesystem::path> paths{read_canonical_path()};
        _skip_blanks();
        while (_peek() == ',') {
            _skip();
            paths.emplace_back(read_canonical_path());
            _skip_blanks();
        }
        _match('}');
        return paths;
    } else if (c == 't' || c == 'f') {// bools
        std::vector<bool> bools{_read_bool()};
        _skip_blanks();
        while (_peek() == ',') {
            _skip();
            _skip_blanks();
            bools.emplace_back(_read_bool());
            _skip_blanks();
        }
        _match('}');
        return bools;
    }
    // should be empty list
    _match('}');
    return {};
}

void SceneParser::_parse_node_body(SceneGraphNode &node) const noexcept {
    _skip_blanks();
    _match('{');
    _skip_blanks();
    while (_peek() != '}') {
        auto property_name = _read_identifier();
        _skip_blanks();
        if (auto c = _peek(); c == ':') {// inline node
            _skip();
            _skip_blanks();
            auto impl_type = _read_identifier();
            auto inline_node = new SceneGraphNode{{}, std::string{":"}.append(impl_type)};
            _parse_node_body(*inline_node);
            node._inline_nodes.emplace_back(inline_node);
            node._add_property(property_name, std::vector{const_cast<const SceneGraphNode *>(inline_node)});
        } else {// value list
            node._add_property(property_name, _parse_value_list());
        }
        _skip_blanks();
    }
    _match('}');
}

double SceneParser::_read_number() const noexcept {
    auto p = _source.data() + _cursor;
    auto p_end = static_cast<char *>(nullptr);
    if (auto number = strtod(p, &p_end); p != p_end) {
        _skip(p_end - p);
        return number;
    }
    GR_ERROR_WITH_LOCATION("Failed to parse number.");
}

bool SceneParser::_read_bool() const noexcept {
    using namespace std::string_view_literals;
    if (_peek() == 't') {
        for (auto x : "true"sv) { _match(x); }
        return true;
    }
    for (auto x : "false"sv) { _match(x); }
    return false;
}

}// namespace gr
