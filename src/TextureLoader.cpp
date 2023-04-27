#include "TextureLoader.hpp"

SDL_Texture* TextureUtil::LoadTexture(const char* p_Texpath, SDL_Renderer* p_Renderer) {
	//Load iamge as surface
	SDL_Surface* Temp = IMG_Load(p_Texpath);
	//Create texture from the surface
	SDL_Texture* Texture = SDL_CreateTextureFromSurface(p_Renderer, Temp);
	//Free the surface and return texture
	SDL_FreeSurface(Temp);
	return Texture;
}