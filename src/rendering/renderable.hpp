#pragma once

#include <atomic>
#include <cstdint>
#include <variant>
#include <SDL.h>

namespace rendering {
    class render_job;
    
    class renderable {
        static std::atomic<std::uint32_t> _next_id;

        std::uint32_t _id;

    public:
        renderable() : _id{ _next_id++ } {}

        [[nodiscard]] std::uint32_t id() const {
            return _id;
        }

        virtual void render(SDL_Renderer* renderer) = 0;

        virtual ~renderable() = default;
    };
}
