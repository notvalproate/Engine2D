#include "Components.hpp"

PolygonCollider::PolygonCollider(GameObject* gameObj) 
	: Collider(gameObj), 
	m_Points({Vector2D(-0.5, -0.5), Vector2D(-0.5, 0.5), Vector2D(0.5, 0.5), Vector2D(0.5, -0.5)}) {
	
}

std::unique_ptr<Component> PolygonCollider::Clone() const {
	return std::make_unique<PolygonCollider>(*this);
}

void PolygonCollider::SetPoints(const std::vector<Vector2D>& points, const Vector2D& offset, const double rotation) {

}

void PolygonCollider::AddPoint(const Vector2D& point, const double rotation) {

}

b2Shape* PolygonCollider::GetShape(bool useOffset) const {
	b2PolygonShape* boxShape = new b2PolygonShape();
	b2Vec2 offset(0, 0);

	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	std::vector<std::vector<b2Vec2>> polygons{};

	for (std::size_t i = 0; i < m_Points.size(); i++) {
		if (polygons.size() <= i / 8) {
			polygons.push_back({});
		}

		polygons.back().push_back(b2Vec2(m_Points[i].x, m_Points[i].y));
	}

	boxShape->Set(&polygons[0][0], polygons[0].size());

	return boxShape;
}