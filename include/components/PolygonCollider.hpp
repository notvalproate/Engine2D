#pragma once

#include "Collider.hpp"

namespace engine2d {

class PolygonCollider final : public Collider {
public:
	~PolygonCollider() override;

	void CreatePrimitive(const std::size_t sides, const Vector2D& scale, const Vector2D& offset);
	void SetPoints(const std::vector<Vector2D>& points, const Vector2D& offset);
	std::vector<Vector2D> GetPoints() const { return m_Points; }
	std::size_t GetPathCount() const { return m_Paths.size(); }

	void SetDensity(const double density) override;
private:
	PolygonCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	b2Shape* GetShape(bool useOffset = false) const override;
	void ResetShape() override;

	void AttachRigidBody(RigidBody* rigidBody) override;
	void DeatachRigidBody() override;

	bool ContainsConcavity(const std::vector<Vector2D>& points) const;
	void CreateFixturesOnBody(b2Body* body);
	void ReducePointsToPaths();

	std::vector<Vector2D> m_Points;
	std::vector<std::vector<b2Vec2>> m_Paths;
	std::vector<b2Fixture*> m_FixtureVector;

	friend class GameObject;
	friend class RigidBody;
};

} // namespace engine2d