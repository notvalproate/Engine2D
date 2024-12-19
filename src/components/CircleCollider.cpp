#include "Components.hpp"

CircleCollider::CircleCollider(GameObject* gameObj) : Collider(gameObj), m_Radius(1) {
	auto spriteRenderer = gameObj->GetComponent<SpriteRenderer>();
	if (spriteRenderer) {
		double x = spriteRenderer->GetWorldDimensions().x * gameObj->transform.scale.x;
		double y = spriteRenderer->GetWorldDimensions().y * gameObj->transform.scale.y;
		m_Radius = x > y ? x / 2.0 : y / 2.0;
	}
}

std::unique_ptr<Component> CircleCollider::Clone() const {
	return std::make_unique<CircleCollider>(*this);
}

void CircleCollider::SetTransform(const double radius, const Vector2D& offset, const double rotation) {
	m_Radius = radius;
	m_Offset = offset;
	m_Rotation = rotation;

	ResetShape();
}

b2Shape* CircleCollider::GetShape(bool useOffset) const {
	b2CircleShape* circleShape = new b2CircleShape();
	b2Vec2 offset(0, 0);

	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	circleShape->m_p = offset;
	circleShape->m_radius = m_Radius;

	return circleShape;
}