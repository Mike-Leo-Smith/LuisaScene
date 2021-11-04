//
// Created by Mike Smith on 2021/2/2.
//

#pragma once

#include <iostream>
#include <string_view>
#include <sstream>

namespace gr {

namespace detail {

static constexpr auto LOG_LEVEL_VERBOSE = 4u;
static constexpr auto LOG_LEVEL_INFO = 3u;
static constexpr auto LOG_LEVEL_WARNING = 2u;
static constexpr auto LOG_LEVEL_ERROR = 1u;

[[nodiscard]] uint32_t current_log_level() noexcept;

template<typename OS, typename... Args>
inline void log_to(OS &target, std::string_view type, Args &&...args) noexcept {
    std::ostringstream ss;
    ss << type;
    (ss << ... << std::forward<Args>(args));
    ss << "\n";
    target << ss.str();
}

}

template<typename... Args>
inline void log_verbose(Args &&...args) noexcept {
    if (detail::current_log_level() >= detail::LOG_LEVEL_VERBOSE) {
        detail::log_to(std::cerr, "[DEBUG] ", std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void log_info(Args &&...args) noexcept {
    if (detail::current_log_level() >= detail::LOG_LEVEL_INFO) {
        detail::log_to(std::cerr, "[INFO] ", std::forward<Args>(args)...);
    }
}

template<typename... Args>
inline void log_warning(Args &&...args) noexcept {
    if (detail::current_log_level() >= detail::LOG_LEVEL_WARNING) {
        detail::log_to(std::cerr, "[WARN] ", std::forward<Args>(args)...);
    }
}

template<typename... Args>
[[noreturn]] inline void log_error(Args &&...args) noexcept {
    if (detail::current_log_level() >= detail::LOG_LEVEL_ERROR) {
        detail::log_to(std::cerr, "[ERROR] ", std::forward<Args>(args)...);
    }
    std::abort();
}

void log_level_verbose() noexcept;
void log_level_info() noexcept;
void log_level_warning() noexcept;
void log_level_error() noexcept;

}// namespace gr

#ifndef NDEBUG
#define GR_VERBOSE(fmt, ...) \
    ::gr::log_verbose(__VA_ARGS__)
#else
#define GR_VERBOSE(...)
#endif

#define GR_INFO(...) \
    ::gr::log_info(__VA_ARGS__)
#define GR_WARNING(...) \
    ::gr::log_warning(__VA_ARGS__)
#define GR_ERROR(...) \
    ::gr::log_error(__VA_ARGS__)

#define GR_VERBOSE_WITH_LOCATION(...) \
    GR_VERBOSE(__VA_ARGS__ __VA_OPT__(,) "[", __FILE__, ":", __LINE__, "]")
#define GR_INFO_WITH_LOCATION(...) \
    GR_INFO(__VA_ARGS__ __VA_OPT__(,) "[", __FILE__, ":", __LINE__, "]")
#define GR_WARNING_WITH_LOCATION(...) \
    GR_WARNING(__VA_ARGS__ __VA_OPT__(,) "[", __FILE__, ":", __LINE__, "]")
#define GR_ERROR_WITH_LOCATION(...) \
    GR_ERROR(__VA_ARGS__ __VA_OPT__(,) "[", __FILE__, ":", __LINE__, "]")
