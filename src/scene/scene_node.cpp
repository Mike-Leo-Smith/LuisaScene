//
// Created by Mike Smith on 2021/9/1.
//

#include <core/logging.h>
#include <scene/scene_node.h>

namespace gr {

SceneGraphNode::value_list_variant SceneGraphNode::property(std::string_view name) const noexcept {
    if (auto iter = _properties.find(name);
        iter != _properties.cend()) {
        return iter->second;
    }
    return {};
}

void SceneGraphNode::_add_property(std::string_view name, SceneGraphNode::value_list_variant value_list) noexcept {
    if (!_properties.emplace(name, std::move(value_list)).second) {
        GR_WARNING_WITH_LOCATION(
            "Property '", name, "' is overwritten.");
    }
}

void SceneGraphNode::print(std::ostream &os, size_t indent) const noexcept {

    auto flags = os.flags();
    os << std::boolalpha;

    auto print_indent = [&os](auto indent) noexcept {
        for (auto i = 0u; i < indent; i++) { os << "  "; }
    };

    if (auto impl_type = impl_type_identifier(); !impl_type.empty()) {
        os << _name << ": " << impl_type;
    }
    os << " {";
    if (!_properties.empty()) {
        os << "\n";
        for (auto &&[name, value] : _properties) {
            print_indent(indent + 1u);
            os << name;
            std::visit(
                [name = std::string_view{name}, &os, indent](auto &&value_list) {
                    using type = std::remove_cvref_t<decltype(value_list)>;
                    if constexpr (std::disjunction_v<
                                      std::is_same<type, std::vector<bool>>,
                                      std::is_same<type, std::vector<double>>,
                                      std::is_same<type, std::vector<std::filesystem::path>>>) {
                        os  << " { " << value_list.front();
                        for (auto i = 1u; i < value_list.size(); i++) {
                            os << ", " << value_list[i];
                        }
                        os << " }\n";
                    } else if constexpr (std::is_same_v<type, std::vector<const SceneGraphNode *>>) {
                        if (value_list.size() == 1u && !value_list.front()->is_global()) {
                            value_list.front()->print(os, indent + 1u);
                        } else {
                            os  << " { @" << value_list.front()->name();
                            for (auto i = 1u; i < value_list.size(); i++) {
                                os << ", @" << value_list[i]->name();
                            }
                            os << " }\n";
                        }
                    }
                },
                value);
        }
        print_indent(indent);
    }
    os << "}\n";
    os.flags(flags);
}

std::string_view SceneGraphNode::base_type_identifier() const noexcept {
    if (auto p = type_identifier().find(':');
        p != std::string_view::npos) {
        return type_identifier().substr(0u, p);
    }
    return {};
}

std::string_view SceneGraphNode::impl_type_identifier() const noexcept {
    if (auto p = type_identifier().find(':');
        p != std::string_view::npos) {
        return type_identifier().substr(p + 1u);
    }
    return {};
}

}// namespace gr
