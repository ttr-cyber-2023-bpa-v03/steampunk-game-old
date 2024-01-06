#include "content_provider.hpp"
#include <filesystem>
#include "platform/current.hpp"
#include "util/debug.hpp"

namespace assets {
    content_provider::content_provider() {
        // Get directory of executable
        const fs::path exec_path{ platform::executable_path() };

        // Get assets directory then ensure it exists
        _assets_path = exec_path.parent_path() / "assets";
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
            // Idk where linux fonts are so you'll just have to symlink or copy them
            // I can implement this later but it's not a priority

            // Additionally, it might not be the best idea to just use the system fonts.
            // That means we'd have to tell the user to install packages, which many
            // users might not be comfortable with. I'll have to think about this.

            // For now, only the top if statement is in use.
            // I'm on Linux and I just have fonts ready to go in the assets directory.

            // Btw I just removed the windows code for this because I didn't find it
            // useful.

            throw std::runtime_error("Font does not exist");
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
