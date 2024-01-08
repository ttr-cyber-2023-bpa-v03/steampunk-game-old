#include "render_job.hpp"

#include <iostream>
#include <queue>

#include "SDL_render.h"
#include "SDL_video.h"
#include "game/world.hpp"
#include "util/logger.hpp"
#include "sched/runner.hpp"
#include "sched/worker.hpp"

namespace rendering {
    SDL_Window* render_job::window() const {
        if (_window == nullptr)
            throw std::runtime_error("SDL_Window is null");
        return _window.get();
    }

    SDL_Renderer* render_job::renderer() const {
        if (_renderer == nullptr)
            throw std::runtime_error("SDL_Renderer is null");
        return _renderer.get();
	}

    void render_job::add_renderable(const std::shared_ptr<renderable>& renderable) {
        _renderables.emplace(renderable->id(), renderable);
    }

    void render_job::init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            throw std::runtime_error("SDL_Init failed");

        // Create window and check if it was created successfully
        _window.reset(SDL_CreateWindow(
            _name.c_str(),
            SDL_WINDOWPOS_UNDEFINED, 
            SDL_WINDOWPOS_UNDEFINED, 
            static_cast<int>(_window_size.x),
            static_cast<int>(_window_size.y), 
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN
        ));
        if (_window == nullptr)
            throw std::runtime_error("SDL_CreateWindow failed");

        // Create renderer and check if it was created successfully
        _renderer.reset(SDL_CreateRenderer(_window.get(), -1, SDL_RENDERER_ACCELERATED));
        if (_renderer == nullptr)
            throw std::runtime_error("SDL_CreateRenderer failed");
    }

    void render_job::poll_events() {
        SDL_Event event;

        while (SDL_PollEvent(&event) != 0) {
            // Handle different types of events
            switch (event.type) {
            case SDL_QUIT:
                // Stop the scheduler when the window is closed
                game::world::instance()->scheduler->signal_stop();
                break;
            case SDL_KEYDOWN:
                game::world::instance()->key_down->fire(event.key.keysym.sym);
                break;
            default:
                // Handle other events
                break;
            }
        }
    }

    void render_job::present() {
        const auto renderer = _renderer.get();

        // Pre-render
        for (const auto& renderable : _renderables | std::views::values) 
            renderable->pre_render_if_marked(*this);

        // Clear
        SDL_SetRenderDrawColor(renderer, 127, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render/copy
        for (const auto& renderable : _renderables | std::views::values) 
            renderable->render(*this);
        
        // Present
        SDL_RenderPresent(renderer);
    }

    void render_job::execute() {
        if (!_initialized) {
            init(); // Required to init here since SDL requires the thread to be the same as the one that created the window
            _initialized = true;
        }
        poll_events();
        present();
    }
}
