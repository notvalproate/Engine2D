#include "components/Components.hpp"
#include "GameObject.hpp"

namespace engine2d {

BoxCollider::BoxCollider(GameObject* gameObj) : Collider(gameObj), m_Dimensions(Vector2D::one) { 
	auto spriteRenderer = gameObj->GetComponent<SpriteRenderer>();
	if (spriteRenderer) {
		m_Dimensions.x = spriteRenderer->GetWorldDimensions().x * gameObj->transform.scale.x;
		m_Dimensions.y = spriteRenderer->GetWorldDimensions().y * gameObj->transform.scale.y;
	}
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::SetTransform(const Vector2D& dimensions, const Vector2D& offset, const double rotation) {
	m_Dimensions = dimensions;
	m_Offset = offset;
	m_Rotation = rotation;

	ResetShape();
}

b2Shape* BoxCollider::GetShape(bool useOffset) const {
	b2PolygonShape* boxShape = new b2PolygonShape();
	b2Vec2 offset(0, 0);
	
	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	boxShape->SetAsBox(
		m_Dimensions.x / 2.0,
		m_Dimensions.y / 2.0,
		offset,
		-m_Rotation * M_PI / 180.0
	);

	return boxShape;
}

} // namespace engine2d