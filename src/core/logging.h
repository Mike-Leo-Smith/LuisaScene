//
// Created by Mike Smith on 2021/2/2.
//

#pragma once

#include <string_view>

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

namespace gr {

template<typename... Args>
inline void log_verbose(Args &&...args) noexcept { spdlog::debug(std::forward<Args>(args)...); }

template<typename... Args>
inline void log_info(Args &&...args) noexcept { spdlog::info(std::forward<Args>(args)...); }

template<typename... Args>
inline void log_warning(Args &&...args) noexcept { spdlog::warn(std::forward<Args>(args)...); }

template<typename... Args>
[[noreturn]] inline void log_error(Args &&...args) noexcept {
    spdlog::error(std::forward<Args>(args)...);
    std::abort();
}

void log_level_verbose() noexcept;
void log_level_info() noexcept;
void log_level_warning() noexcept;
void log_level_error() noexcept;

}// namespace gr

#ifndef NDEBUG
#define GR_VERBOSE(fmt, ...) \
    ::gr::log_verbose(FMT_STRING(std::string_view{fmt}), ##__VA_ARGS__)
#else
#define GR_VERBOSE(...)
#endif

#define GR_INFO(fmt, ...) \
    ::gr::log_info(FMT_STRING(std::string_view{fmt}) __VA_OPT__(, ) __VA_ARGS__)
#define GR_WARNING(fmt, ...) \
    ::gr::log_warning(FMT_STRING(std::string_view{fmt}), ##__VA_ARGS__)
#define GR_ERROR(fmt, ...) \
    ::gr::log_error(FMT_STRING(std::string_view{fmt}), ##__VA_ARGS__)

#define GR_VERBOSE_WITH_LOCATION(fmt, ...) \
    GR_VERBOSE(fmt " [{}:{}]", ##__VA_ARGS__, __FILE__, __LINE__)
#define GR_INFO_WITH_LOCATION(fmt, ...) \
    GR_INFO(fmt " [{}:{}]", ##__VA_ARGS__, __FILE__, __LINE__)
#define GR_WARNING_WITH_LOCATION(fmt, ...) \
    GR_WARNING(fmt " [{}:{}]", ##__VA_ARGS__, __FILE__, __LINE__)
#define GR_ERROR_WITH_LOCATION(fmt, ...) \
    GR_ERROR(fmt " [{}:{}]", ##__VA_ARGS__, __FILE__, __LINE__)
