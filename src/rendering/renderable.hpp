#pragma once

#include "game/object.hpp"

#include <atomic>
#include <cstdint>
#include <memory>
#include <variant>
#include <SDL.h>

namespace rendering {
    class render_job;

    class renderable : public game::object {
        static std::atomic<std::uint32_t> _next_id;

        std::uint32_t _id;

        std::atomic<bool> _marked_for_pre_render{};

        void pre_render_if_marked(const render_job& rj) {
            if (_marked_for_pre_render) {
                _marked_for_pre_render = false;
                pre_render(rj);
            }
        }

    public:
        void mark_pre_render() {
            _marked_for_pre_render = true;
        }

        renderable() : game::object("renderable") {
            _id = _next_id++;
        }

        [[nodiscard]] std::uint32_t id() {
            return _id;
        }

        virtual void pre_render(const render_job& rj) {};

        virtual void render(const render_job& rj) = 0;

        // Friend classes
		friend class render_job;
    };
}
