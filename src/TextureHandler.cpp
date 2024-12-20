#include "Core.hpp"
#include "SDL_image.h"

SDL_Texture* TextureHandler::LoadTexture(const std::filesystem::path& texPath) const {
	if (!std::filesystem::exists(texPath)) {
		std::cerr << "File path to sprite provided is invalid! Path: " << texPath << std::endl;
		return nullptr;
	}

	return IMG_LoadTexture(Object::RenderingPipeline.m_Renderer, texPath.string().c_str());
}

SDL_Point TextureHandler::GetTextureSize(SDL_Texture* texture) const {
	SDL_Point size{};
	SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
	return size;
}