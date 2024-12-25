#pragma once

#include "Collider.hpp"

namespace engine2d {

// ADD ABILITY TO DISABLE BOXCOLLIDER
class BoxCollider final : public Collider {
public:
	void SetTransform(const Vector2D& dimensions, const Vector2D& offset, const double rotation);
	Vector2D GetSize() const { return m_Dimensions; }

private:
	BoxCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	b2Shape* GetShape(bool useOffset = false) const override;

	Vector2D m_Dimensions;

	friend class GameObject;
	friend class RigidBody;
};

} // namespace engine2d