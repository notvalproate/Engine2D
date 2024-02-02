#include "Components.hpp"
#include "SDL_image.h"

SpriteRenderer::SpriteRenderer(GameObject* gameObject) 
	: Component(gameObject), m_Sprite(nullptr), m_Dimensions({0, 0}), m_SortingLayer("Default") 
{
	RenderingPipeline.AddGameObjectToRenderer(gameObject);
}

std::unique_ptr<Component> SpriteRenderer::Clone() const {
	return std::make_unique<SpriteRenderer>(*this);
}

void SpriteRenderer::SetSprite(const char* spritePath) {
	m_Sprite = TextureManager.LoadTexture(spritePath);
	SDL_Point size = TextureManager.GetTextureSize(m_Sprite);

	m_Dimensions.x = size.x;
	m_Dimensions.y = size.y;
}

void SpriteRenderer::SetSortingLayer(const std::string_view layerName) {
	if (RenderingPipeline.SetSortingLayer(gameObject, layerName, m_SortingLayer)) {
		m_SortingLayer = layerName;
		return;
	}

	m_SortingLayer = "Default";
}

void SpriteRenderer::Render() const {
	RenderingPipeline.RenderSprite(m_Sprite, m_Dimensions, transform);
}