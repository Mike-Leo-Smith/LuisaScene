#include <iostream>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <fstream>

/*
 * #include "something.luisa"
 *
 * Camera camera : Pinhole {
 *   fov {}
 * }
 *
 * Shape shape : Mesh {
 *   file {}
 * }
 *
 * render {
 *
 * }
 *
 */

class SceneNode {

private:
    std::vector<std::unique_ptr<SceneNode>> _local_nodes;

};

class Scene {

private:
    std::filesystem::path _base_folder;
    std::unordered_map<std::string, std::unique_ptr<SceneNode>> _nodes;

public:


};

class FileReader {

private:
    std::filesystem::path _path;
    std::FILE *_handle{nullptr};

private:
    void _close() noexcept {
        if (_handle != nullptr) {
            fclose(_handle);
            _path.clear();
            _handle = nullptr;
        }
    }

public:
    FileReader() noexcept = default;
    explicit FileReader(const std::filesystem::path &path) noexcept
        : _path{std::filesystem::canonical(path)},
          _handle{fopen(_path.c_str(), "r")} {
        if (_handle == nullptr) {

        }
    }



    [[nodiscard]] decltype(auto) path() const noexcept { return (_path); }
    [[nodiscard]] auto get() {
        auto c = fgetc(_handle);
        if (auto valid = std::isalnum(c) || c == '_' || c == '"' || c == '\'' || std::isblank(c); !valid) {

        }
    }


};

class SceneParser {

private:
    mutable std::vector<std::filesystem::path> _path_stack;
    mutable std::vector<std::ifstream> _stream_stack;

    void _clear() const noexcept {
        _path_stack.clear();
        _stream_stack.clear();
    }

    [[nodiscard]] const std::filesystem::path &_current_directory() const noexcept {
        return _path_stack.back();
    }

    [[nodiscard]] std::ifstream &_current_stream() const noexcept {
        return _stream_stack.back();
    }

public:
    SceneParser() noexcept = default;

    [[nodiscard]] std::unique_ptr<Scene> parse(const std::filesystem::path &main_file) const noexcept {

    }

};

int main() {

    std::ifstream file{"test.luisa"};
    file.get();

    return 0;
}
