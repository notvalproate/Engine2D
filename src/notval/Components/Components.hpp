#pragma once
#include "../Core.hpp"

class SpriteRenderer;

class SpriteRenderer final : public Component {
public:
	void SetSprite(const char* spritePath);
	void SetSortingLayer(const std::string_view layerName);

private:
	SpriteRenderer(GameObject* gameObject);
	std::unique_ptr<Component> Clone() const;

	void Update();
	void Render() const;

	SDL_Texture* m_Sprite;
	SDL_Rect m_SrcRect, m_DestRect;
	std::string m_SortingLayer;

	friend class GameObject;
};

class BoxCollider final : public Behaviour {
	
};