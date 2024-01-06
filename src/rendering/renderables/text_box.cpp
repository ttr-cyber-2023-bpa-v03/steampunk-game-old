#include "text_box.hpp"
#include "util/debug.hpp"

namespace rendering {
    SDL_Color hsv2rgb(float h, float s, float v) {
        h = std::fmod(h, 1.0f);
        int i = static_cast<int>(h * 6);
        float f = (h * 6) - i;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (i % 6) {
            case 0: return SDL_Color{ static_cast<std::uint8_t>(v * 255), static_cast<std::uint8_t>(t * 255), static_cast<std::uint8_t>(p * 255), 255 };
            case 1: return SDL_Color{ static_cast<std::uint8_t>(q * 255), static_cast<std::uint8_t>(v * 255), static_cast<std::uint8_t>(p * 255), 255 };
            case 2: return SDL_Color{ static_cast<std::uint8_t>(p * 255), static_cast<std::uint8_t>(v * 255), static_cast<std::uint8_t>(t * 255), 255 };
            case 3: return SDL_Color{ static_cast<std::uint8_t>(p * 255), static_cast<std::uint8_t>(q * 255), static_cast<std::uint8_t>(v * 255), 255 };
            case 4: return SDL_Color{ static_cast<std::uint8_t>(t * 255), static_cast<std::uint8_t>(p * 255), static_cast<std::uint8_t>(v * 255), 255 };
            default: return SDL_Color{ static_cast<std::uint8_t>(v * 255), static_cast<std::uint8_t>(p * 255), static_cast<std::uint8_t>(q * 255), 255 };
        }
    }
}