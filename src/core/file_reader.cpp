//
// Created by Mike Smith on 2021/9/1.
//

#include <limits>

#include <core/file_reader.h>
#include <core/logging.h>

namespace gr {

void FileReader::_close() noexcept {
    if (*this) {
        fclose(_handle);
        _path.clear();
        _handle = nullptr;
    }
}

FileReader::FileReader(const std::filesystem::path &path) noexcept
    : _path{std::filesystem::canonical(path)},
      _handle{fopen(_path.string().c_str(), "r")} {}

char FileReader::get() noexcept {
    auto c = fgetc(_handle);
    if (c > std::numeric_limits<char>::max()) [[unlikely]] {
        GR_ERROR_WITH_LOCATION("Invalid character: 0x{:08x}.", c);
    }
    return static_cast<char>(c);
}

FileReader::FileReader(FileReader &&another) noexcept
    : _path{std::move(another._path)},
      _handle{another._handle} {
    another._handle = nullptr;
}

FileReader &FileReader::operator=(FileReader &&rhs) noexcept {
    if (&rhs != this) {
        _close();
        _path = std::move(rhs._path);
        _handle = rhs._handle;
        rhs._handle = nullptr;
    }
    return *this;
}

void FileReader::unget() noexcept {
    if (fseek(_handle, -1, SEEK_CUR) != 0) [[unlikely]] {
        GR_ERROR_WITH_LOCATION(
            "Failed to un-get character from file ", _path, ".");
    }
}

size_t FileReader::position() const noexcept {
    fpos_t p;
    if (fgetpos(_handle, &p) != 0) {
        GR_ERROR_WITH_LOCATION(
            "Failed to get current position in file ", _path, ".");
    }
    return p;
}

void FileReader::set_position(size_t p) noexcept {
    if (auto fp = static_cast<fpos_t>(p);
        fsetpos(_handle, &fp) != 0) [[unlikely]] {
        GR_ERROR_WITH_LOCATION(
            "Failed to set position to ", p, " in file ", _path, ".");
    }
}

}// namespace gr
