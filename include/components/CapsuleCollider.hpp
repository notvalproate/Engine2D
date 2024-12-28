#pragma once

#include "Collider.hpp"

namespace engine2d {

enum class CapsuleDirection {
	Vertical = 0,
	Horizontal = 1
};

class CapsuleCollider final : public Collider {
public:
	void SetTransform(const Vector2D& size, const CapsuleDirection direction, const Vector2D& offset);

	Vector2D GetSize() const { return m_Size; }
	CapsuleDirection GetDirection() const { return m_Direction; }
private:
	CapsuleCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	std::vector<b2Shape*> GetShapes(bool useOffset = false) const override;

	Vector2D m_Size;
	CapsuleDirection m_Direction;

	friend class GameObject;
	friend class RigidBody;
};

} // namespace engine2d