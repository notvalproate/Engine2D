#pragma once
#include "SDL_image.h"
#include "SDL.h"

namespace notval::TextureUtil {
	SDL_Texture* LoadTexture(const char* texpath, SDL_Renderer* renderer);
}