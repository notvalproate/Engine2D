#pragma once

#include <optional>

#include "Component.hpp"

namespace engine2d {

struct PhysicsMaterial {
	double bounciness = 0.0;
	double friction = 0.3;

	explicit inline PhysicsMaterial() = default;
	explicit inline PhysicsMaterial(const double bounciness, const double friction) : bounciness(bounciness), friction(friction) { }
};

enum class RigidBodyType {
	Static = b2_staticBody,
	Kinematic = b2_kinematicBody,
	Dynamic = b2_dynamicBody
};

class Collider;

// TOTAL FORCE AND TOTAL TORQUE ARE ALWAYS ZERO IN THE UPDATE LOOPS OF BEHAVIOURS
class RigidBody final : public Component {
public:
	~RigidBody();

	void AddForce(const Vector2D& force);
	void AddForceAtPosition(const Vector2D& force, const Vector2D& position);
	void AddRelativeForce(Vector2D force);
	void AddTorque(const double force);

	void SetMass(const float mass);
	void SetAutoMass(const bool use);
	void SetMaterial(const PhysicsMaterial& material);
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
	PhysicsMaterial GetMaterial() const;
	Vector2D GetVelocity() const;
	float GetAngularVelocity() const;
	RigidBodyType GetBodyType() const;
	Vector2D GetCentreOfMass() const;
	std::vector<Collider*> GetAttachedColliders() const;
	bool IsAwake() const;
	bool IsSleeping() const;

	double linearDrag;
	double angularDrag;
	Vector2D totalForce;
	double totalTorque;

private:
	RigidBody(GameObject* gameObj);

	void Update() override;
	void AttachCollider(Collider* collider);

	void AddDrag();
	void AddAngularDrag();
	void ApplyTotalForces();
	void ResetMassToInternalMass();

	b2Body* m_Body;

	double m_Mass;
	bool m_AutoMassEnabled{};
	bool m_FreezeRotation{};
	PhysicsMaterial m_Material;
	std::vector<Collider*> m_AttachedColliders;
	std::optional<b2Fixture*> m_SensorFixture;

	friend class GameObject;
	friend class Collider;
	friend class PolygonCollider;
	friend class EdgeCollider;
	friend class CapsuleCollider;
	friend class Scene;
};

} // namespace engine2d