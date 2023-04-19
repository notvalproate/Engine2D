#pragma once
#include "SDL_image.h"
#include "SDL.h"

namespace TextureUtil {
	SDL_Texture* LoadTexture(const char* p_Texpath, SDL_Renderer* p_Renderer);
}