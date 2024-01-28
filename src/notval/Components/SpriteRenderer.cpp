#include "Components.hpp"
#include "SDL_image.h"

SpriteRenderer::SpriteRenderer(GameObject* gameObject) : Component(gameObject), m_Sprite(nullptr), m_SrcRect({0, 0, 0, 0}), m_DestRect({0, 0, 0, 0}) { }

std::unique_ptr<Component> SpriteRenderer::Clone() const {
	return std::make_unique<SpriteRenderer>(*this);
}

void SpriteRenderer::SetSprite(const char* spritePath) {
	m_Sprite = TextureManager.LoadTexture(spritePath);
	SDL_Point size = TextureManager.GetTextureSize(m_Sprite);

	m_SrcRect.w = size.x;
	m_SrcRect.h = size.y;
	m_DestRect.w = size.x * transform->scale.x;
	m_DestRect.h = size.y * transform->scale.y;
}

void SpriteRenderer::Update() {
	m_DestRect.x = transform->position.x;
	m_DestRect.y = transform->position.y;
	m_DestRect.w = m_SrcRect.w * transform->scale.x;
	m_DestRect.h = m_SrcRect.h * transform->scale.y;
}
void SpriteRenderer::Render() const {
	RenderingPipeline.RenderSprite(m_Sprite, m_SrcRect, m_DestRect);
}