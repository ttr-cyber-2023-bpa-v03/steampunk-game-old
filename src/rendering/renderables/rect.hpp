#pragma once

#include <SDL.h>
#include <glm/vec2.hpp>

#include "rendering/renderable.hpp"

namespace rendering {
    class render_job;

    class rect final : public renderable {
    public:
        glm::vec2 position{};

        glm::vec2 size{};

        SDL_Color color{ 255, 255, 255, 255 };

        bool fill{};

        void render(SDL_Renderer* renderer) override {
            const SDL_Rect rect{
               static_cast<int>(position.x),
               static_cast<int>(position.y),
               static_cast<int>(size.x),
               static_cast<int>(size.y)
            };

            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            if (fill)
                SDL_RenderFillRect(renderer, &rect);
            else
                SDL_RenderDrawRect(renderer, &rect);
        }
    };
}
