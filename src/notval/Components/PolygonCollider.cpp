#include "Components.hpp"

PolygonCollider::PolygonCollider(GameObject* gameObj) 
	: Collider(gameObj), 
	m_Points({Vector2D(-0.5, -0.5), Vector2D(-0.5, 0.5), Vector2D(0.5, 0.5), Vector2D(0.5, -0.5)}) {
	
}

std::unique_ptr<Component> PolygonCollider::Clone() const {
	return std::make_unique<PolygonCollider>(*this);
}

void PolygonCollider::SetPoints(const std::vector<Vector2D>& points) {
	if (ContainsConcavity(points)) {
		std::cout << "Polygon provided has concavity" << std::endl;
		return;
	}

	m_Points = points;
	ResetShape();
}

void PolygonCollider::SetOffset(const Vector2D& offset) {
	m_Offset = offset;

	ResetShape();
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

bool PolygonCollider::ContainsConcavity(const std::vector<Vector2D>& points) const {
	bool hasConcavity = false;
	std::size_t prevPrev = 0;
	std::size_t prev = 1;
	std::size_t curr = 2;

	while (true) {
		const Vector2D& first = points[prevPrev];
		const Vector2D& second = points[prev];
		const Vector2D& current = points[curr];

		double dy = (second.y - first.y);
		double dx = (second.x - first.x);
		bool infiniteSlope = dx == 0;
		bool upward = dy > 0;

		if (infiniteSlope) {
			if (upward) {
				if (current.x < second.x) {
					hasConcavity = true;
					break;
				}
			}
			else {
				if (current.x > second.x) {
					hasConcavity = true;
					break;
				}
			}
		}
		else {
			double m = (dy / dx);
			double c = second.y - (m * second.x);

			double y0 = (m * current.x) + c;

			bool above = current.y > y0;

			bool forward = dx > 0;
			above = forward ? above : !above;

			if (above) {
				hasConcavity = true;
				break;
			}
		}

		if (curr == 0) {
			break;
		}

		prevPrev++;
		prev++;
		curr = (curr + 1) % points.size();
	}

	return hasConcavity;
}

void PolygonCollider::TriangulatePoints() {
	
}