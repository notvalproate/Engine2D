#pragma once
#include "SDL_image.h"
#include "SDL.h"

namespace TextureUtil {
	SDL_Texture* LoadTexture(const char* p_Texpath, SDL_Renderer* p_Renderer) {
		SDL_Surface* Temp = IMG_Load(p_Texpath);
		SDL_Texture* Texture = SDL_CreateTextureFromSurface(p_Renderer, Temp);
		SDL_FreeSurface(Temp);
		return Texture;
	}
}