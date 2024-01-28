#include "Components.hpp"
#include "SDL_image.h"

SpriteRenderer::SpriteRenderer(GameObject* gameObject) : Component(gameObject), m_Sprite(nullptr), m_SrcRect({0, 0, 0, 0}), m_DestRect({0, 0, 0, 0}) { }

std::unique_ptr<Component> SpriteRenderer::Clone() const {
	return std::make_unique<SpriteRenderer>(*this);
}

void SpriteRenderer::SetSprite(const char* spritePath) {
	m_Sprite = TextureManager.LoadTexture(spritePath);
	SDL_Point size = TextureManager.GetTextureSize(m_Sprite);

	m_SrcRect.w = m_DestRect.w = size.x;
	m_SrcRect.h = m_DestRect.h = size.y;
}

void SpriteRenderer::Update() {
	m_DestRect.x = transform->position.x;
	m_DestRect.y = transform->position.y;
}
void SpriteRenderer::Render() const {

}