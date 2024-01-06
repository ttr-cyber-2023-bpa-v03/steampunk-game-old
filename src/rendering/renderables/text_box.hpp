#pragma once

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <glm/vec2.hpp>

#include "SDL_pixels.h"
#include "assets/content_provider.hpp"
#include "rendering/renderable.hpp"
#include "util/debug.hpp"

namespace rendering {
    class render_job;

    SDL_Color hsv2rgb(float h, float s, float v);

    class text_box final : public renderable {
        std::shared_ptr<TTF_Font> _font;

        util::unique_sdl<SDL_Surface> _surface;

        util::unique_sdl<SDL_Texture> _texture;

        SDL_Color _color{};

        float h = 0.0f;

        std::string _text{};

    public:
        glm::vec2 position{};

        void set_color(const SDL_Color& color) {
            this->_color = color;
        }

        void set_text(const std::string& text) {
            _text = text;
        }

        void set_font(const std::string& name, const std::uint32_t size) {
            _font = assets::content_provider::get()->get_font(name, size);
        }

        void render(SDL_Renderer* renderer) override {
            _surface = util::unique_sdl<SDL_Surface>{ TTF_RenderText_Blended(_font.get(), _text.c_str(), _color) };
            if (_surface == nullptr)
                throw std::runtime_error("gort failure: surface");

            _texture = util::unique_sdl<SDL_Texture>{ SDL_CreateTextureFromSurface(renderer, _surface.get()) };
            if (_texture == nullptr)
                throw std::runtime_error("gort failure: texture");
            
            const SDL_Rect rect{ static_cast<int>(position.x), static_cast<int>(position.y), _surface->w, _surface->h };
            SDL_RenderCopy(renderer, _texture.get(), nullptr, &rect);
        
            /*h += 0.01f;
            _color = hsv2rgb(h, 1.0f, 1.0f);
            if (h >= 1.0f) h = 0.0f;*/
        }
    };
}
