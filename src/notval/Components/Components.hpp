#pragma once
#include "../Core.hpp"

class SpriteRenderer final : public Component {
public:
	~SpriteRenderer();

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

// TOTAL FORCE AND TOTAL TORQUE ARE ALWAYS ZERO IN THE UPDATE LOOPS OF BEHAVIOURS
class RigidBody final : public Component {
public:
	~RigidBody();

	void AddForce(const Vector2D& force);
	void AddForceAtPosition(const Vector2D& force, const Vector2D& position);
	void AddRelativeForce(Vector2D force);
	void AddTorque(const double force);

	void SetMass(const float mass);
	void SetVelocity(const Vector2D& vel);
	void SetAngularVelocity(const float vel);
	void SetPosition(const Vector2D& pos);
	void SetRotation(const double rotation);
	void SetBodyType(const RigidBodyType type);
	void SetGravityScale(const double scale);
	void FreezeRotation(const bool set);
	void WakeUp();
	void Sleep();

	float GetMass() const;
	Vector2D GetVelocity() const;
	float GetAngularVelocity() const;
	RigidBodyType GetBodyType() const;
	Vector2D GetCentreOfMass() const;
	std::vector<BoxCollider*> GetAttachedColliders() const;
	bool IsAwake() const;
	bool IsSleeping() const;

	double drag;
	double angularDrag;
	Vector2D totalForce;
	double totalTorque;

	b2Body* m_Body;
private:
	RigidBody(GameObject* gameObj);

	void Update() override;
	void AttachCollider(BoxCollider* collider);

	void AddDrag();
	void AddAngularDrag();
	void ApplyTotalForces();

	std::vector<BoxCollider*> m_AttachedColliders;
	std::optional<b2Fixture*> m_SensorFixture;

	friend class GameObject;
	friend class BoxCollider;
	friend class Scene;
};

// ADD ABILITY TO DISABLE BOXCOLLIDER
// MASS OF THE RIGIDBODY NOT USED WHEN IT ATTACHES TO A RIGID BODY
class BoxCollider final : public Behaviour {
public:
	~BoxCollider();

	void SetTransform(const Vector2D& dimensions, const Vector2D& offset, const double rotation);
	Vector2D GetCenter() const { return transform->position + m_Offset; }
	Vector2D GetSize() const { return m_Dimensions; }

	RigidBody* attachedRigidBody;

private:
	BoxCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	void Update();

	void UpdateStaticPosition();
	void AttachRigidBody(RigidBody* rigidBody);
	void DeatachRigidBody();
	void RemoveFixtureFromMap();
	void AddFixtureToMap();

	b2Fixture* m_Fixture;
	Vector2D m_Dimensions;
	Vector2D m_Offset;
	double m_Rotation;

	Vector2D m_CurrentPosition; 
	std::optional<b2Body*> m_StaticBody;

	friend class GameObject;
	friend class RigidBody;
};