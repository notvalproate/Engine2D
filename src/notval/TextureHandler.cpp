#include "Core.hpp"
#include "SDL_image.h"

SDL_Texture* TextureHandler::LoadTexture(const char* texpath) const {
	SDL_Surface* temp = IMG_Load(texpath);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(RenderingPipeline.m_Renderer, temp);
	SDL_FreeSurface(temp);
	return texture;
}

SDL_Point TextureHandler::GetTextureSize(SDL_Texture* texture) const {
	SDL_Point size{};
	SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
	return size;
}