#include <iostream>

#include "components/PolygonCollider.hpp"
#include "GameObject.hpp"

namespace engine2d {

PolygonCollider::PolygonCollider(GameObject* gameObj) 
	: Collider(gameObj), 
	m_Points({Vector2D(-0.5, -0.5), Vector2D(-0.5, 0.5), Vector2D(0.5, 0.5), Vector2D(0.5, -0.5)}),
	m_Paths({ {b2Vec2(-0.5, -0.5), b2Vec2(-0.5, 0.5), b2Vec2(0.5, 0.5), b2Vec2(0.5, -0.5)} })
{
	
}

std::unique_ptr<Component> PolygonCollider::Clone() const {
	return std::make_unique<PolygonCollider>(*this);
}

void PolygonCollider::CreatePrimitive(const std::size_t sides, const Vector2D& scale, const Vector2D& offset) {
	if (sides <= 2) {
		std::cout << "Polygon primitive must have more than 2 sides" << std::endl;
	}

	m_Points.clear();

	float angle = 2 * M_PI / sides;

	for (std::size_t i = 0; i < sides; i++) {
		float x = cos(i * angle);
		float y = sin(i * angle);
		m_Points.push_back( Vector2D(y * scale.x, x * scale.y) );
	}

	m_Offset = offset;
	ReducePointsToPaths();
	ResetShape();
}

void PolygonCollider::SetPoints(const std::vector<Vector2D>& points, const Vector2D& offset) {
	if (points.size() < 3) {
		std::cout << "Polygon provided does not have enough points! (less than 3)" << std::endl;
		return;
	}

	if (ContainsConcavity(points)) {
		std::cout << "Polygon provided has concavity(s)" << std::endl;
		return;
	}

	m_Points = points;
	m_Offset = offset;
	ReducePointsToPaths();
	ResetShape();
}

std::vector<b2Shape*> PolygonCollider::GetShapes(bool) const {
	std::vector<b2Shape*> parts;

	for (const auto& path : m_Paths) {
		b2PolygonShape* boxShape = new b2PolygonShape();
		boxShape->Set(&path[0], path.size());
		parts.push_back(boxShape);
	}

	return parts;
}

// O(n) solution to find out if the list of points contain a concavity. Algorithm terminates as soon as concavity is detected.
// Requires points to be supplied as clockwise winding ordered boundary points of a polygon.
bool PolygonCollider::ContainsConcavity(const std::vector<Vector2D>& points) const {
	// Concavity test is only valid for polygons with more than 3 points.
	if (points.size() <= 3) {
		return false;
	}

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
		bool goingUp = dy > 0;

		bool upLeft = goingUp && current.x < second.x;
		bool downRight = !goingUp && current.x > second.x;

		if (infiniteSlope) {
			if ((upLeft || downRight)) {
				return true;
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
				return true;
			}
		}

		if (curr == 0) {
			break;
		}

		prevPrev++;
		prev++;
		curr = (curr + 1) % points.size();
	}

	return false;
}


void PolygonCollider::ReducePointsToPaths() {
	m_Paths.clear();

	for (std::size_t prevEnd = 1; prevEnd < m_Points.size() - 1; ) {
		std::size_t endIndex = std::min(prevEnd + (b2_maxPolygonVertices - 2), m_Points.size() - 1);

		m_Paths.push_back({
			b2Vec2(m_Points[0].x + m_Offset.x, m_Points[0].y + m_Offset.y),
			b2Vec2(m_Points[prevEnd].x + m_Offset.x, m_Points[prevEnd].y + m_Offset.y)
			});

		for (std::size_t i = prevEnd + 1; i <= endIndex; i++) {
			m_Paths.back().push_back(b2Vec2(m_Points[i].x + m_Offset.x, m_Points[i].y + m_Offset.y));
		}

		prevEnd = endIndex;
	}
}

} // namespace engine2d