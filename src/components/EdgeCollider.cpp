#include "components/EdgeCollider.hpp"
#include "GameObject.hpp"

namespace engine2d {

EdgeCollider::EdgeCollider(GameObject* gameObj) 
	: Collider(gameObj), 
	m_Points( { Vector2D(-0.5, 0), Vector2D(0.5, 0) } )
{
	
}

std::unique_ptr<Component> EdgeCollider::Clone() const {
	return std::make_unique<EdgeCollider>(*this);
}

void EdgeCollider::SetPoints(const std::vector<Vector2D>& points, const Vector2D& offset) {
	m_Offset = offset;
	m_Points = points;

	ResetShape();
}

void EdgeCollider::Reset() {
	SetPoints({ Vector2D(-0.5, 0), Vector2D(0, 0.5) }, Vector2D::zero);
}

void EdgeCollider::UpdateMassData() const {
	if (!m_AttachedRigidBody) {
		return;
	}

	b2MassData massdata;

	massdata.mass = m_AttachedRigidBody->GetMass();
	massdata.center = m_AttachedRigidBody->m_Body->GetLocalCenter();
	massdata.I = m_AttachedRigidBody->m_Body->GetInertia();
	massdata.I = massdata.I ? massdata.I : 1;

	m_AttachedRigidBody->m_Body->SetMassData(&massdata);
}

std::vector<b2Shape*> EdgeCollider::GetShapes(bool useOffset) const {
	b2ChainShape* forwardChain = new b2ChainShape();
	b2ChainShape* reverseChain = new b2ChainShape();
	b2Vec2 offset(0, 0);

	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	std::vector<b2Vec2> forwardPoints;
	forwardPoints.reserve(m_Points.size());

	for (const auto& point : m_Points) {
		forwardPoints.push_back(b2Vec2(point.x + offset.x, point.y + offset.y));
	}

	std::vector<b2Vec2> reversePoints(forwardPoints);
	std::reverse(reversePoints.begin(), reversePoints.end());

	forwardChain->CreateChain(&forwardPoints[0], forwardPoints.size(), forwardPoints.front(), forwardPoints.back());
	reverseChain->CreateChain(&reversePoints[0], reversePoints.size(), reversePoints.front(), reversePoints.back());

	return { forwardChain, reverseChain };
}

} // namespace engine2d