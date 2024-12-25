#pragma once

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

struct Collision {
	Collider* collider;
	Collider* otherCollider;

	RigidBody* rigidBody;
	RigidBody* otherRigidBody;

	Transform* transform;

	GameObject* gameObject;
};

class Collider : public Behaviour {
public:
	virtual ~Collider();

	virtual void SetDensity(const double density);
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
	virtual void ResetShape();
	void ResetDensity();

	virtual void AttachRigidBody(RigidBody* rigidBody);
	virtual void DeatachRigidBody();

	void CreateColliderOnRigidBody(const b2Shape* colShape);
	void CreateStaticBody();
	void CreateStaticCollider(const b2Shape* colShape);
	void DestroyStaticCollider();
	b2BodyDef GetStaticBodyDef() const;
	b2FixtureDef GetFixtureDef(const b2Shape* colShape) const;

	virtual b2Shape* GetShape(bool useOffset = false) const = 0;

	bool IsCollidingWith(Collider* collider) const;
	void RemoveCollisionWith(Collider* collider);
	
	bool IsTriggeringWith(Collider* collider) const;
	void RemoveTriggerWith(Collider* collider);

	RigidBody* m_AttachedRigidBody;

	b2Fixture* m_Fixture;
	Vector2D m_Offset;
	double m_Rotation;

	Bounds m_BoundingBox;
	std::optional<PhysicsMaterial> m_Material;

	double m_Density;

	Vector2D m_CurrentPosition;
	std::optional<b2Body*> m_StaticBody;

	std::vector<Collision> m_CurrentCollisions{};
	std::vector<Collision> m_CurrentTriggers{};
	bool m_IsTrigger{};

	friend class GameObject;
	friend class RigidBody;
	friend class PhysicsHandler;
};

} // namespace engine2d
