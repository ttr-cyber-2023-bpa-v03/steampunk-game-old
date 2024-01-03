#include "content_provider.hpp"

namespace assets {
    content_provider::content_provider() {
        // Get directory of executable
#   if defined(_WIN32)
        std::string executable_path_str(MAX_PATH, '\0');
        const auto program_path_len = GetModuleFileNameA(nullptr, executable_path_str.data(), MAX_PATH);
        if (program_path_len == 0)
            throw std::runtime_error("GetModuleFileNameA failed");
        executable_path_str.resize(program_path_len);
#   elif defined(__linux__)
        std::string executable_path_str(PATH_MAX, '\0');
        const auto program_path_len = readlink("/proc/self/exe", executable_path_str.data(), PATH_MAX);
        if (program_path_len == -1)
            throw std::runtime_error("readlink failed");
        executable_path_str.resize(program_path_len);
#   endif

        // Set executable path
        _executable_path = fs::path{ executable_path_str };

        // Get assets directory then ensure it exists
        _assets_path = _executable_path.parent_path() / "assets";
        if (!fs::exists(_assets_path)) {
            if (!fs::create_directory(_assets_path))
                throw std::runtime_error("Failed to create assets directory");
        }

        // Initialize SDL stuff for assets
        if (TTF_Init() == -1)
            throw std::runtime_error("TTF_Init failed");
    }

    content_provider::~content_provider() {
        TTF_Quit();
    }

    content_provider* content_provider::_singleton{};
    content_provider* content_provider::get() {
        if (_singleton == nullptr)
            _singleton = new content_provider{};
        return _singleton;
    }

    std::shared_ptr<TTF_Font> content_provider::get_font(const std::string& relative_path, const std::uint32_t size) {
        const auto key = _get_font_key(relative_path, size);
        if (_font_cache.contains(key))
            return _font_cache[key].lock();

        auto font_path = _assets_path / "fonts" / relative_path;
        if (!fs::exists(font_path)) {
#           if defined(_WIN32)
            font_path = fs::path{ u8R"(C:\Windows\Fonts\)" } / relative_path;
            if (!fs::exists(font_path))
                throw std::runtime_error("Font does not exist");
#           else
                // Idk where linux fonts are so you'll just have to symlink or copy them
                // I can implement this later but it's not a priority

                // Additionally, it might not be the best idea to just use the system fonts.
                // That means we'd have to tell the user to install packages, which many
                // users might not be comfortable with. I'll have to think about this.

                // For now, only the top if statement is in use.
                // I'm on Linux and I just have fonts ready to go in the assets directory.

                throw std::runtime_error("Font does not exist");
#           endif
        }

        const auto font = TTF_OpenFont(font_path.generic_string().c_str(), static_cast<int>(size));
        if (font == nullptr)
            throw std::runtime_error("man the fonts just dont be fonting");

        // Create a shared pointer with a custom deleter that calls the appropriate SDL dealloc method
        // This is actually really cool because any object that uses this will reuse it, but once
        // this pointer is no longer used, the SDL object will be destroyed without having to explicitly
        // deallocate it. Super cool, super clean, super safe.
        std::shared_ptr<TTF_Font> ptr{ font, util::sdl_destroyer{} };
        _font_cache.emplace(key, ptr);

        // Transfer ownership of the pointer from the callee to the caller
        return std::move(ptr); // NOLINT omg shut up clang-tidy IM TRANSFERRING OWNERSHIP OKAY?
    }
}
