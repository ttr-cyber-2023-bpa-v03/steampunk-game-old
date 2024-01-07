#pragma once

#include <SDL.h>
#include <glm/vec2.hpp>
#include <functional>

#include "SDL_rect.h"
#include "SDL_render.h"
#include "game/object.hpp"
#include "game/write_job.hpp"
#include "game/world.hpp"
#include "glm/ext/vector_float2.hpp"
#include "rendering/renderable.hpp"
#include "rendering/render_job.hpp"

namespace rendering {
    class rect final : public renderable {
        SDL_Color _color{ 255, 255, 255, 255 };

        SDL_Rect _rect { 0, 0, 0, 0 };

        using sdl_render_func = int(*)(SDL_Renderer*, const SDL_Rect*);
        sdl_render_func _draw_rect;

    public:
        // Color property
        SG_IMPL_GET_WRAP(color);
        SG_IMPL_SET_WRAP(color);

        // Get the position of the rectangle
        SG_IMPL_GET(glm::vec2, position, {
            return glm::vec2{
                static_cast<float>(_rect.x),
                static_cast<float>(_rect.y)
            };
        });

        // Set the position of the rectangle
        SG_IMPL_SET(glm::vec2, position, {
            _rect.x = static_cast<int>(value.x);
            _rect.y = static_cast<int>(value.y);
        });

        // Get the size of the rectangle
        SG_IMPL_GET(glm::vec2, size, {
            return glm::vec2{
                static_cast<float>(_rect.w),
                static_cast<float>(_rect.h)
            };
        });

        // Set the size of the rectangle
        SG_IMPL_SET(glm::vec2, size, {
            _rect.w = static_cast<int>(value.x);
            _rect.h = static_cast<int>(value.y);
        });

        // Get whether the rectangle should be filled
        SG_IMPL_GET(bool, fill, {
            return _draw_rect == SDL_RenderFillRect;
        });

        // Set whether the rectangle should be filled
        SG_IMPL_SET(bool, fill, {
            _draw_rect = value ? SDL_RenderFillRect : SDL_RenderDrawRect;
        });
        
        rect() = default;

        rect(glm::vec2 position, glm::vec2 size, SDL_Color color, bool fill = true) {
            set_position(position);
            set_size(size);
            set_color(color);
            set_fill(fill);
        }

        void render(const render_job& rj) override {
            // Set the color to draw with
            const auto& [ r, g, b, a ] = _color;
            SDL_SetRenderDrawColor(*rj, r, g, b, a);

            // Draw the rectangle
            _draw_rect(*rj, &_rect);
        }
    };
}
