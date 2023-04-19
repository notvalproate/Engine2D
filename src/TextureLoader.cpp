#include "TextureLoader.hpp"

SDL_Texture* TextureUtil::LoadTexture(const char* p_Texpath, SDL_Renderer* p_Renderer) {
	SDL_Surface* Temp = IMG_Load(p_Texpath);
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(p_Renderer, Temp);
	SDL_FreeSurface(Temp);
	return Texture;
}