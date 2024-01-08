#pragma once

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include <glm/vec2.hpp>

#include "game/object.hpp"
#include "game/world.hpp"
#include "game/write_job.hpp"
#include "util/logger.hpp"
#include "rendering/render_job.hpp"
#include "SDL_pixels.h"
#include "assets/content_provider.hpp"
#include "rendering/renderable.hpp"
#include "assets/font_init.hpp"

namespace rendering {
    class render_job;

    SDL_Color hsv2rgb(float h, float s, float v);

    class text_box final : public renderable {
        std::shared_ptr<TTF_Font> _font;

        util::unique_sdl<SDL_Surface> _surface;

        util::unique_sdl<SDL_Texture> _texture;

        SDL_Color _color{ 255, 255, 255, 255 };

        SDL_Rect _rect{ 0, 0, 0, 0 };

        float h = 0.0f;

        std::string _text{};

    public:
        // Color property
        SG_IMPL_GET_WRAP(color);
        SG_IMPL_SET(SDL_Color, color, {
            _color = value;
            mark_pre_render();
        });

        // Get the text of the text box
        SG_IMPL_GET(std::string, text);

        // Set the text of the text box
        SG_IMPL_SET(std::string, text, {
            _text = value;
            mark_pre_render();
        });

        // Get the position of the text box
        SG_IMPL_GET(glm::vec2, position, {
            return glm::vec2{
                static_cast<float>(_rect.x),
                static_cast<float>(_rect.y)
            };
        });

        // Set the position of the text box
        SG_IMPL_SET(glm::vec2, position, {
            _rect.x = static_cast<int>(value.x);
            _rect.y = static_cast<int>(value.y);
        });

        // Set the font of the text box
        SG_IMPL_SET(assets::font_init, font, {
            auto& [ path, size ] = value;
            _font = assets::content_provider::get()->get_font(path, size);
            mark_pre_render();
        });

        void pre_render(const render_job& rj) override;

        void render(const render_job& rj) override;
    };
}
