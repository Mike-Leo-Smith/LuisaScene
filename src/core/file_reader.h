//
// Created by Mike Smith on 2021/9/1.
//

#pragma once

#include <cstdio>
#include <filesystem>
#include <deque>

namespace gr {

class FileReader {

private:
    std::filesystem::path _path;
    std::FILE *_handle{nullptr};

private:
    void _close() noexcept;

public:
    FileReader() noexcept = default;
    explicit FileReader(const std::filesystem::path &path) noexcept;
    FileReader(FileReader &&another) noexcept;
    FileReader &operator=(FileReader &&rhs) noexcept;

    FileReader(const FileReader &) noexcept = delete;
    FileReader &operator=(const FileReader &) noexcept = delete;

    [[nodiscard]] decltype(auto) path() const noexcept { return (_path); }
    [[nodiscard]] explicit operator bool() const noexcept { return _handle != nullptr; }

    void close() noexcept { _close(); }
    [[nodiscard]] char get() noexcept;
    void unget() noexcept;
    [[nodiscard]] size_t position() const noexcept;
    void set_position(size_t p) noexcept;
};

}// namespace gr
