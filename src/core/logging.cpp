//
// Created by Mike Smith on 2021/2/11.
//

#include <core/logging.h>

namespace gr {

[[nodiscard]] static uint32_t &get_log_level() noexcept {
    static uint32_t level = detail::LOG_LEVEL_INFO;
    return level;
}

namespace detail {
[[nodiscard]] uint32_t current_log_level() noexcept { return get_log_level(); }
}

void log_level_verbose() noexcept { get_log_level() = detail::LOG_LEVEL_VERBOSE; }
void log_level_info() noexcept { get_log_level() = detail::LOG_LEVEL_INFO; }
void log_level_warning() noexcept { get_log_level() = detail::LOG_LEVEL_WARNING; }
void log_level_error() noexcept { get_log_level() = detail::LOG_LEVEL_ERROR; }

}// namespace luisa
