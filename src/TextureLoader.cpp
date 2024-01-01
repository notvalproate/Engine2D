#include "TextureLoader.hpp"

SDL_Texture* TextureUtil::LoadTexture(const char* texpath, SDL_Renderer* renderer) {
	//Load iamge as surface
	SDL_Surface* temp = IMG_Load(texpath);
	//Create texture from the surface
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, temp);
	//Free the surface and return texture
	SDL_FreeSurface(temp);
	return texture;
}