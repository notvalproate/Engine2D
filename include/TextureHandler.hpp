#pragma once

#include <filesystem>

#include <SDL.h>

namespace engine2d {

class TextureHandler {
public:
    SDL_Texture* LoadTexture(const std::filesystem::path& texPath) const;
    SDL_Point GetTextureSize(SDL_Texture* texture) const;

private:
    TextureHandler() = default;

    friend class Object;
};

} // namespace engine2d