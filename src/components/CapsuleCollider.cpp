#include "components/CapsuleCollider.hpp"
#include "GameObject.hpp"

namespace engine2d {

CapsuleCollider::CapsuleCollider(GameObject* gameObj)
	: Collider(gameObj),
	m_Size(1, 1),
	m_Direction(CapsuleDirection::Vertical)
{

}

std::unique_ptr<Component> CapsuleCollider::Clone() const {
	return std::make_unique<CapsuleCollider>(*this);
}

void CapsuleCollider::SetTransform(const Vector2D& size, const CapsuleDirection direction, const Vector2D& offset) {
	m_Size = size;
	m_Direction = direction;
	m_Offset = offset;

	ResetShape();
}

std::vector<b2Shape*> CapsuleCollider::GetShapes(bool useOffset) const {
	b2PolygonShape* boxShape = new b2PolygonShape();
	b2Vec2 offset(0, 0);

	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	// For the middle box

	boxShape->SetAsBox(
		m_Size.x / 2.0, 
		m_Size.y / 2.0,
		b2Vec2(offset.x, offset.y),
		-m_Rotation * M_PI / 180.0
	);

	// Radius and offset for circles

	float radius = m_Size.x / 2.0;
	b2Vec2 circleOffset(0, m_Size.y / 2.0);

	if (m_Direction == CapsuleDirection::Horizontal) {
		radius = m_Size.y / 2.0;
		offset = b2Vec2(m_Size.x / 2.0, 0);
	}

	// For the top circle

	b2CircleShape* circleTop = new b2CircleShape();

	circleTop->m_radius = radius;
	circleTop->m_p = offset + b2Vec2(m_Offset.x, m_Offset.y);

	// For the bottom circle

	b2CircleShape* circleBottom = new b2CircleShape();

	circleBottom->m_radius = radius;
	circleBottom->m_p = -offset + b2Vec2(m_Offset.x, m_Offset.y);

	return { boxShape, circleTop, circleBottom };
}

} // namespace engine2d