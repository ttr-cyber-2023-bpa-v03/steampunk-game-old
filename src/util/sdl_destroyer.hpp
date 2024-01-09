#pragma once

#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>

namespace util {
    struct sdl_destroyer {
	    void operator()(SDL_Window* window) const noexcept {
	        if (window)
				SDL_DestroyWindow(window);
	    }

	    void operator()(SDL_Surface* surface) const noexcept {
	        if (surface)
				SDL_FreeSurface(surface);
	    }

		void operator()(SDL_Renderer* renderer) const noexcept {
			// TODO: Temporary bugfix, but theres a timing issue with the cleanup
			//       of the renderer and the window. This is a temporary fix until
	    	//if (renderer) 
			//	SDL_DestroyRenderer(renderer);
		}

		void operator()(SDL_Texture* texture) const noexcept {
	    	if (texture)
				SDL_DestroyTexture(texture);
		}

		void operator()(TTF_Font* font) const noexcept {
	    	if (font)
				TTF_CloseFont(font);
		}
	};

	// Just a simple alias
	// Saves a couple of migraines
	template <class T>
	using unique_sdl = std::unique_ptr<T, sdl_destroyer>;
}