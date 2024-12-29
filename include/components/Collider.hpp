#pragma once

#include <unordered_map>

#include "Behaviour.hpp"
#include "RigidBody.hpp"

namespace engine2d {
    
struct Bounds {
public:
	Vector2D center;
	Vector2D extents;
	Vector2D max;
	Vector2D min;
	Vector2D size;
};

struct Collision;

class Collider : public Behaviour {
public:
	virtual ~Collider();

	void SetDensity(const double density);
	void SetMaterial(const PhysicsMaterial& material);
	void SetFriction(const double friction);
	void SetBounciness(const double bounciness);
	void SetAsTrigger(const bool sensor);

	Vector2D GetCenter() const { return transform->position + m_Offset; }
	Vector2D GetOffset() const { return m_Offset; }
	Bounds GetBounds() const { return m_BoundingBox; }
	double GetDensity() const;
	std::optional<PhysicsMaterial> GetMaterial() const;
	double GetFriction() const;
	double GetBounciness() const;
	RigidBody* GetAttachedRigidBody() const { return m_AttachedRigidBody; }

protected:
	Collider(GameObject* gameObj);

	void Awake() override;
	void Update() override final;
	void UpdateBounds();
	void UpdateStaticPosition();
	void ResetShape();
	void ResetDensity();

	void AttachRigidBody(RigidBody* rigidBody);
	void DeatachRigidBody();

	void CreateColliderOnRigidBody(const std::vector<b2Shape*>& colShapes);
	void CreateStaticBody();
	void CreateStaticCollider(const std::vector<b2Shape*>& colShapes);
	void DestroyStaticCollider();
	b2BodyDef GetStaticBodyDef() const;
	b2FixtureDef GetFixtureDef(const b2Shape* colShape) const;

	virtual void UpdateMassData() const { };
	virtual std::vector<b2Shape*> GetShapes(bool useOffset = false) const = 0;

	// Collision Tracking

	bool IsCollidingWith(Collider* collider) const;
	bool IsTriggeringWith(Collider* collider) const;

	RigidBody* m_AttachedRigidBody;

	std::vector<b2Fixture*> m_Fixtures;
	Vector2D m_Offset;
	double m_Rotation;

	Bounds m_BoundingBox;
	std::optional<PhysicsMaterial> m_Material;

	double m_Density;

	Vector2D m_CurrentPosition;
	std::optional<b2Body*> m_StaticBody;

	bool m_IsTrigger{};

	std::unordered_map<Collider*, std::size_t> m_Contacts{};
	std::unordered_map<Collider*, std::size_t> m_Overlaps{};

	friend class GameObject;
	friend class RigidBody;
	friend class PhysicsHandler;
};

struct Collision {
	explicit Collision(Collider* collider, Collider* otherCollider)
		: 
		collider(collider), 
		otherCollider(otherCollider), 
		rigidBody(collider->GetAttachedRigidBody()), 
		otherRigidBody(otherCollider->GetAttachedRigidBody()),
		transform(collider->transform),
		gameObject(collider->gameObject)
		{ }

	Collider* collider;
	Collider* otherCollider;

	RigidBody* rigidBody;
	RigidBody* otherRigidBody;

	Transform* transform;

	GameObject* gameObject;
};

} // namespace engine2d
