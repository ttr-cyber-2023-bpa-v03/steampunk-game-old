#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <filesystem>

#include "util/sdl_destroyer.hpp"

#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

namespace assets {
    class content_provider {
        std::filesystem::path _assets_path;

        // A cache of textures, mapped by their name
        std::unordered_map<std::string, std::weak_ptr<SDL_Texture>> _texture_cache;

        // A cache of surfaces, mapped by their name
        std::unordered_map<std::string, std::weak_ptr<SDL_Surface>> _surface_cache;

        // A cache of fonts, mapped by their path and size
        std::unordered_map<std::string, std::weak_ptr<TTF_Font>> _font_cache;

        static std::string get_font_key(const std::string& path, const std::uint32_t size) {
            // This probably isn't ideal, but this is what came to mind, and also idk why
            // anyone would name their file "font.ttf;size=12" or something like that.
            // So far, there won't be any issues with this, so I'll leave it for now.
            return path + ";size=" + std::to_string(size);
        }

    public:
        content_provider();

        ~content_provider();

        // Access the singleton instance
        static content_provider* get();

        // Pull a font from the cache or load it if it doesn't exist
        std::shared_ptr<TTF_Font> get_font(const std::string& relative_path, std::uint32_t size);
    };
}
