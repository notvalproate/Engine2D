#include "Core.hpp"
#include "SDL_image.h"

SDL_Texture* TextureHandler::LoadTexture(const char* texpath) const {
	return IMG_LoadTexture(Object::RenderingPipeline.m_Renderer, texpath);
}

SDL_Point TextureHandler::GetTextureSize(SDL_Texture* texture) const {
	SDL_Point size{};
	SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
	return size;
}