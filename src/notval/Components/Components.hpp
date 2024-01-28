#pragma once
#include "../Core.hpp"

class SpriteRenderer;

class SpriteRenderer final : public Component {
public:
	void SetSprite(const char* spritePath);

private:
	SpriteRenderer(GameObject* gameObject);
	std::unique_ptr<Component> Clone() const;

	void Update();
	void Render() const;

	SDL_Texture* m_Sprite;
	SDL_Rect m_SrcRect, m_DestRect;

	friend class GameObject;
};