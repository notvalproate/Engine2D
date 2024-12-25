#pragma once

#include "Collider.hpp"

namespace engine2d {

class CircleCollider final : public Collider {
public:
	void SetTransform(const double radius, const Vector2D& offset, const double rotation);
	double GetRadius() const { return m_Radius; }

private:
	CircleCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	b2Shape* GetShape(bool useOffset = false) const override;

	double m_Radius;

	friend class GameObject;
	friend class RigidBody;
};

} // namespace engine2d