#pragma once
#include "../Core.hpp"

class SpriteRenderer final : public Component {
public:
	void SetSprite(const char* spritePath);
	void SetSortingLayer(const std::string_view layerName);
	void SetPixelsPerUnit(const uint16_t pixelsPerUnit);

private:
	SpriteRenderer(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	void Render() const override;

	SDL_Texture* m_Sprite;
	Vector2D m_Dimensions;
	uint16_t m_PixelsPerUnit;
	std::string m_SortingLayer;

	friend class GameObject;
};

class RigidBody final : public Component {
private:
	RigidBody(GameObject* gameObj);

	void Update() override;

	b2Body* m_Body;

	friend class GameObject;
	friend class BoxCollider;
};

class BoxCollider final : public Behaviour {
public:
	RigidBody* attachedRigidBody;

private:
	BoxCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	void Update();

	void UpdateStaticPosition();
	void AttachRigidBody(RigidBody* rigidBody);

	b2Fixture* m_Fixture;
	Vector2D m_CurrentPosition;

	std::optional<b2Body*> m_StaticBody;

	friend class GameObject;
	friend class RigidBody;
};