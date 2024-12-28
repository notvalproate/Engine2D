#pragma once

#include "Collider.hpp"

namespace engine2d {

class PolygonCollider final : public Collider {
public:
	void CreatePrimitive(const std::size_t sides, const Vector2D& scale, const Vector2D& offset);
	void SetPoints(const std::vector<Vector2D>& points, const Vector2D& offset);
	std::vector<Vector2D> GetPoints() const { return m_Points; }
	std::size_t GetPathCount() const { return m_Paths.size(); }
private:
	PolygonCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	std::vector<b2Shape*> GetShapes(bool useOffset = false) const override;

	bool ContainsConcavity(const std::vector<Vector2D>& points) const;
	void ReducePointsToPaths();

	std::vector<Vector2D> m_Points;
	std::vector<std::vector<b2Vec2>> m_Paths;

	friend class GameObject;
	friend class RigidBody;
};

} // namespace engine2d