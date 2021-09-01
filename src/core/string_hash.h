//
// Created by Mike Smith on 2021/9/2.
//

#pragma once

#include <string>
#include <string_view>

namespace gr {

struct StringHash {
    using is_transparent = void;
    using hash_type = std::hash<std::string_view>;
    [[nodiscard]] auto operator()(const char *str) const noexcept { return hash_type{}(str); }
    [[nodiscard]] auto operator()(std::string_view str) const noexcept { return hash_type{}(str); }
    [[nodiscard]] auto operator()(std::string const &str) const noexcept { return hash_type{}(str); }
};

}// namespace gr