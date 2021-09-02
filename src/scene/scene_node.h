//
// Created by Mike Smith on 2021/9/1.
//

#pragma once

#include <vector>
#include <tuple>
#include <memory>
#include <variant>
#include <string>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <filesystem>

#include <core/string_hash.h>

namespace gr {

namespace detail {

template<typename...>
using always_false = std::false_type;

template<typename... T>
constexpr auto always_false_v = always_false<T...>::value;

template<template<typename> typename F, typename Tuple>
struct tuple_map {
    static_assert(always_false_v<Tuple>, "Not a tuple.");
};

template<template<typename> typename F, typename... T>
struct tuple_map<F, std::tuple<T...>> {
    using type = std::tuple<F<T>...>;
};

template<template<typename> typename F, typename T>
using tuple_map_t = typename tuple_map<F, T>::type;

template<typename T>
using vector_t = std::vector<T>;

template<typename T>
struct tuple_to_variant {
    static_assert(always_false_v<T>, "Not a tuple.");
};

template<typename... T>
struct tuple_to_variant<std::tuple<T...>> {
    using type = std::variant<std::monostate, T...>;
};

template<typename T>
using tuple_to_variant_t = typename tuple_to_variant<T>::type;

}// namespace detail

class SceneParser;

class SceneNode {

public:
    using value_tuple = std::tuple<bool, double, std::filesystem::path, const SceneNode *>;
    using value_list_variant = detail::tuple_to_variant_t<detail::tuple_map_t<detail::vector_t, value_tuple>>;

private:
    std::string _name;
    std::string _type_identifier;
    std::vector<std::unique_ptr<SceneNode>> _inline_nodes;
    std::unordered_map<std::string, value_list_variant, StringHash, std::equal_to<>> _properties;

private:
    friend class SceneParser;
    SceneNode(std::string name, std::string type) noexcept
        : _name{std::move(name)},
          _type_identifier{std::move(type)} {}
    void _add_property(std::string_view name, value_list_variant value_list) noexcept;

public:
    SceneNode(SceneNode &&) noexcept = delete;
    SceneNode(const SceneNode &) noexcept = delete;
    SceneNode &operator=(SceneNode &&) noexcept = delete;
    SceneNode &operator=(const SceneNode &) noexcept = delete;
    [[nodiscard]] std::string_view name() const noexcept { return _name; }
    [[nodiscard]] std::string_view type_identifier() const noexcept { return _type_identifier; }
    [[nodiscard]] std::string_view base_type_identifier() const noexcept;
    [[nodiscard]] std::string_view impl_type_identifier() const noexcept;
    [[nodiscard]] decltype(auto) properties() const noexcept { return (_properties); }
    [[nodiscard]] value_list_variant property(std::string_view name) const noexcept;
    [[nodiscard]] auto is_global() const noexcept { return !_name.empty(); }

    // for debug
    void print(std::ostream &os, size_t indent = 0u) const noexcept;
};

}// namespace gr
