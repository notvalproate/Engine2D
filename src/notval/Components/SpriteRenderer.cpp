#include "Components.hpp"
#include "SDL_image.h"

SpriteRenderer::SpriteRenderer(GameObject* gameObj)
	: Component(gameObj), m_Sprite(nullptr), m_Dimensions(0, 0), m_PixelsPerUnit(100), m_SortingLayer("Default")
{
	gameObj->scene->AddObjectToSortingLayers(gameObj);
}

SpriteRenderer::~SpriteRenderer() {
	SDL_DestroyTexture(m_Sprite);
}

std::unique_ptr<Component> SpriteRenderer::Clone() const {
	return std::make_unique<SpriteRenderer>(*this);
}

void SpriteRenderer::SetSprite(const char* spritePath) {
	if (m_Sprite) {
		SDL_DestroyTexture(m_Sprite);
	}

	m_Sprite = TextureManager.LoadTexture(spritePath);
	SDL_Point size = TextureManager.GetTextureSize(m_Sprite);

	m_Dimensions.x = size.x;
	m_Dimensions.y = size.y;
}

void SpriteRenderer::SetSortingLayer(const std::string_view layerName) {
	if (gameObject->scene->SetSortingLayer(gameObject, layerName, m_SortingLayer)) {
		m_SortingLayer = layerName;
		return;
	}

	m_SortingLayer = "Default";
}

void SpriteRenderer::SetPixelsPerUnit(const uint16_t pixelsPerUnit) {
	m_PixelsPerUnit = pixelsPerUnit;
}

void SpriteRenderer::Render() const {
	RenderingPipeline.RenderSprite(m_Sprite, m_Dimensions, m_PixelsPerUnit, transform);
}