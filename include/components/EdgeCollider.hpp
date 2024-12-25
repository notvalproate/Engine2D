#pragma once

#include "Collider.hpp"

namespace engine2d {

class EdgeCollider final : public Collider {
public:
	~EdgeCollider() override;

	void SetPoints(const std::vector<Vector2D>& points, const Vector2D& offset);
	void Reset();
	std::vector<Vector2D> GetPoints() const { return m_Points; }
	std::size_t GetPointCount() const { return m_Points.size(); }
	std::size_t GetEdgeCount() const { return m_Points.size() - 1; }

private:
	EdgeCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	void Awake() override;

	b2Shape* GetShape(bool useOffset = false) const override;
	void ResetShape() override;

	void AttachRigidBody(RigidBody* rigidBody) override;
	void DeatachRigidBody() override;

	void CreateFixturesOnBody(b2Body* body);
	void UpdateMassData() const;

	std::vector<Vector2D> m_Points;
	b2Fixture* m_ReverseFixture;

	friend class GameObject;
	friend class RigidBody;
};

} // namespace engine2d