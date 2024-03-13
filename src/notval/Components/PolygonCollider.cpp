#include "Components.hpp"

PolygonCollider::PolygonCollider(GameObject* gameObj) 
	: Collider(gameObj), 
	m_Points({Vector2D(-0.5, -0.5), Vector2D(-0.5, 0.5), Vector2D(0.5, 0.5), Vector2D(0.5, -0.5)}),
	m_ReducedPolygons({ {b2Vec2(-0.5, -0.5), b2Vec2(-0.5, 0.5), b2Vec2(0.5, 0.5), b2Vec2(0.5, -0.5)} }),
	m_FixtureVector({})
{
	
}

std::unique_ptr<Component> PolygonCollider::Clone() const {
	return std::make_unique<PolygonCollider>(*this);
}

void PolygonCollider::SetPoints(const std::vector<Vector2D>& points) {
	if (points.size() < 3) {
		std::cout << "Polygon provided does not have enough points! (less than 3)" << std::endl;
		return;
	}

	if (ContainsConcavity(points)) {
		std::cout << "Polygon provided has concavity(s)" << std::endl;
		return;
	}

	m_Points = points;
	ReducePointsToPolygons();
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

	boxShape->Set(&m_ReducedPolygons[0][0], m_ReducedPolygons[0].size());

	return boxShape;
}

// O(n) solution to find out if the list of points contain a concavity. Algorithm terminates as soon as concavity is detected.
// Requires points to be supplied as clockwise winding ordered boundary points of a polygon.
bool PolygonCollider::ContainsConcavity(const std::vector<Vector2D>& points) const {
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
		curr = ++curr % points.size();
	}

	return false;
}

void PolygonCollider::ReducePointsToPolygons() {
	m_ReducedPolygons.clear();

	for (std::size_t prevEnd = 1; prevEnd < m_Points.size() - 1; ) {
		std::size_t endIndex = std::min(prevEnd + 6, m_Points.size() - 1);

		m_ReducedPolygons.push_back({ 
			b2Vec2(m_Points[0].x, m_Points[0].y), 
			b2Vec2(m_Points[prevEnd].x, m_Points[prevEnd].y) 
		});

		for (std::size_t i = prevEnd + 1; i <= endIndex; i++) {
			m_ReducedPolygons.back().push_back(b2Vec2(m_Points[i].x, m_Points[i].y));
		}

		prevEnd = endIndex;
	}
}

void PolygonCollider::RemoveFixtureFromMap() const {
	auto& sceneColliderMap = gameObject->scene->m_FixtureColliderMap;

	auto it = sceneColliderMap.find(m_Fixture);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}

	for (auto& fixture : m_FixtureVector) {
		auto it = sceneColliderMap.find(fixture);

		if (it != sceneColliderMap.end()) {
			sceneColliderMap.erase(fixture);
		}
	}
}

void PolygonCollider::AddFixtureToMap() {
	for (auto& fixture : m_FixtureVector) {
		gameObject->scene->m_FixtureColliderMap[fixture] = this;
	}

	gameObject->scene->m_FixtureColliderMap[m_Fixture] = this;
}