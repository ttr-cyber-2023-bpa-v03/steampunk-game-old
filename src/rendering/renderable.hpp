#pragma once

#include "game/object.hpp"

#include <atomic>
#include <cstdint>
#include <variant>
#include <SDL.h>

namespace rendering {
    class render_job;

    class renderable : public game::object {
        static std::atomic<std::uint32_t> _next_id;

    public:
        renderable() : game::object("renderable") {
            
        }

        [[nodiscard]] std::uint32_t id() {
            return _next_id++;
        }

        virtual void render(SDL_Renderer* renderer) = 0;

        virtual ~renderable() = default;
    };
}
