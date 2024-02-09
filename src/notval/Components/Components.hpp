#pragma once
#include "../Core.hpp"

class SpriteRenderer final : public Component {
public:
	void SetSprite(const char* spritePath);
	void SetSortingLayer(const std::string_view layerName);
	void SetPixelsPerUnit(const uint16_t pixelsPerUnit);

	inline Vector2D GetPixelDimensions() const { return m_Dimensions; }
	inline Vector2D GetWorldDimensions() const { return m_Dimensions / m_PixelsPerUnit; }

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
	void OnColliderAttach();

	b2Body* m_Body;

	std::optional<b2Fixture*> m_SensorFixture;

	friend class GameObject;
	friend class BoxCollider;
};

class BoxCollider final : public Behaviour {
public:
	void SetTransform(const Vector2D dimensions, const Vector2D offset, const double rotation);

	RigidBody* attachedRigidBody;

private:
	BoxCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	void Update();

	void UpdateStaticPosition();
	void AttachRigidBody(RigidBody* rigidBody);

	b2Fixture* m_Fixture;
	Vector2D m_Dimensions;
	Vector2D m_Offset;
	double m_Rotation;

	Vector2D m_CurrentPosition; 
	std::optional<b2Body*> m_StaticBody;

	friend class GameObject;
	friend class RigidBody;
};