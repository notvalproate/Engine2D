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

enum class RigidBodyType {
	Static = b2_staticBody,
	Kinematic = b2_kinematicBody,
	Dynamic = b2_dynamicBody
};

class RigidBody final : public Component {
public:
	void AddForce(const Vector2D force);
	void AddTorque(const double force);

	void SetMass(const float mass);
	void SetVelocity(const Vector2D vel);
	void SetBodyType(const RigidBodyType type);
	void SetGravityScale(const double scale);
	void FreezeRotation(const bool set);

	float GetMass() const;
	Vector2D GetVelocity() const;
	RigidBodyType GetBodyType() const;
	float GetAngularVelocity() const;
	Vector2D GetCentreOfMass() const;

	double drag;
	double angularDrag;
private:
	RigidBody(GameObject* gameObj);

	void Update() override;
	void OnColliderAttach();

	b2Body* m_Body;
	std::optional<b2Fixture*> m_SensorFixture;

	friend class GameObject;
	friend class BoxCollider;
	friend class Scene;
};

class BoxCollider final : public Behaviour {
public:
	void SetTransform(const Vector2D dimensions, const Vector2D offset, const double rotation);
	Vector2D GetCenter() const { return transform->position + m_Offset; }
	Vector2D GetSize() const { return m_Dimensions; }

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