#pragma once
#include "../Core.hpp"

class SpriteRenderer final : public Component {
public:
	void SetSprite(const char* spritePath);
	void SetSortingLayer(const std::string_view layerName);
	void SetPixelsPerUnit(const uint16_t pixelsPerUnit);

private:
	SpriteRenderer(GameObject* gameObject);
	~SpriteRenderer();
	std::unique_ptr<Component> Clone() const;

	void Render() const override;

	SDL_Texture* m_Sprite;
	Vector2D m_Dimensions;
	uint16_t m_PixelsPerUnit;
	std::string m_SortingLayer;

	friend class GameObject;
};

class BoxCollider final : public Behaviour {
private:
	BoxCollider(GameObject* gameObject);
	std::unique_ptr<Component> Clone() const;

	void Update() override;

	b2Body* m_Body;
	b2Fixture* m_Fixture;

	friend class GameObject;
};