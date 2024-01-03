#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <filesystem>

#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    include <Windows.h>
#    undef WIN32_LEAN_AND_MEAN
#endif

#include "util/sdl_destroyer.hpp"

#include <SDL_render.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

namespace assets {
    namespace fs = std::filesystem;

    class content_provider {
        static content_provider* _singleton;

        fs::path _executable_path;

        fs::path _assets_path;

        std::unordered_map<std::string, std::weak_ptr<SDL_Texture>> _texture_cache;

        std::unordered_map<std::string, std::weak_ptr<SDL_Surface>> _surface_cache;

        std::unordered_map<std::string, std::weak_ptr<TTF_Font>> _font_cache;

        static std::string _get_font_key(const std::string& path, const std::uint32_t size) {
            return path + ";size=" + std::to_string(size);
        }

    public:
        content_provider();

        ~content_provider();

        static content_provider* get();

        std::shared_ptr<TTF_Font> get_font(const std::string& relative_path, std::uint32_t size);
    };
}
