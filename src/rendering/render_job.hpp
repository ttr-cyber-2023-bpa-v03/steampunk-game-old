#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <glm/vec2.hpp>

#include <memory>
#include <ranges>
#include <stdexcept>
#include <unordered_map>

#include "sched/job.hpp"
#include "renderable.hpp"
#include "util/sdl_destroyer.hpp"

#include <tbb/concurrent_hash_map.h>

namespace rendering {
    class render_job final : public sched::job {
		tbb::concurrent_hash_map<std::uint32_t, std::shared_ptr<renderable>> _renderables;
		
		util::unique_sdl<SDL_Window> _window;
		util::unique_sdl<SDL_Renderer> _renderer;

		std::string _name;
		glm::vec2 _window_size;

		bool _initialized{};

		void init();

		static void poll_events();

    	void present();

	public:
		render_job(const std::string_view name, const glm::vec2 window_size) {
			_name = name;
			_window_size = window_size;
		}

		[[nodiscard]] SDL_Window* window() const;

		void add_renderable(const std::shared_ptr<renderable>& renderable);

		void execute() override;
    };
}
