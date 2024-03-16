#include "Components.hpp"

EdgeCollider::EdgeCollider(GameObject* gameObj) : Collider(gameObj), m_Start(Vector2D(-0.5, 0)), m_End(Vector2D(0.5, 0)) {
	
}

std::unique_ptr<Component> EdgeCollider::Clone() const {
	return std::make_unique<EdgeCollider>(*this);
}

void EdgeCollider::SetEdge(const Vector2D& start, const Vector2D& end) {
	m_Start = start;
	m_End = end;

	ResetShape();
}

b2Shape* EdgeCollider::GetShape(bool useOffset) const {
	b2EdgeShape* edgeShape = new b2EdgeShape();
	b2Vec2 offset(0, 0);

	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	edgeShape->SetTwoSided(b2Vec2(m_Start.x, m_Start.y), b2Vec2(m_End.x, m_End.y));

	return edgeShape;
}