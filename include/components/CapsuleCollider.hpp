#pragma once

#include "Collider.hpp"

namespace engine2d {

enum class CapsuleDirection {
	Vertical = 0,
	Horizontal = 1
};

class CapsuleCollider final : public Collider {
public:
	~CapsuleCollider() override;

	void SetTransform(const Vector2D& size, const CapsuleDirection direction, const Vector2D& offset);

	void SetDensity(const double density) override;

	Vector2D GetSize() const { return m_Size; }
	CapsuleDirection GetDirection() const { return m_Direction; }
private:
	CapsuleCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	void Awake() override;

	b2Shape* GetShape(bool useOffset = false) const override;
	void ResetShape() override;

	void AttachRigidBody(RigidBody* rigidBody) override;
	void DeatachRigidBody() override;

	void CreateFixturesOnBody(b2Body* body);

	Vector2D m_Size;
	CapsuleDirection m_Direction;
	b2Fixture* m_UpperSemi;
	b2Fixture* m_LowerSemi;

	friend class GameObject;
	friend class RigidBody;
};

} // namespace engine2d