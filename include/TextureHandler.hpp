#pragma once

#include <filesystem>

#include <SDL.h>

class TextureHandler {
public:
    SDL_Texture* LoadTexture(const std::filesystem::path& texPath) const;
    SDL_Point GetTextureSize(SDL_Texture* texture) const;

private:
    TextureHandler() = default;

    friend class Object;
};