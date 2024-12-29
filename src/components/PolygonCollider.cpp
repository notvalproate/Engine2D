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

	m_Points = points;
	m_Offset = offset;

	if (ContainsConcavity(m_Points)) {
		std::cout << "Polygon provided has concavity(s)" << std::endl;
		m_Paths = Triangulate();
	} else {
		ReducePointsToPaths();
	}

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
// Loren Pechtel: https://stackoverflow.com/a/472001
bool PolygonCollider::ContainsConcavity(const std::vector<Vector2D>& points) const {
	// Concavity test is only valid for polygons with more than 3 points.
	if (points.size() <= 3) {
		return false;
	}

    std::size_t p1 = 0, p2 = 1, p3 = 2;
    bool first = true;
    bool positive = false;

    while(p3 != 1) {
        Vector2D v1 = points[p2] - points[p1];
        Vector2D v2 = points[p3] - points[p2];

        double cross = Vector2D::Cross(v1, v2);

		if (cross == 0) {
			return true;
		}

        if (first) {
            positive = cross > 0;
            first = false;
        } else if (positive != (cross > 0)) {
            return true;
        }

        p1 = p2;
        p2 = p3;
        p3 = (p3 + 1) % points.size();
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


std::vector<std::vector<b2Vec2>> PolygonCollider::Triangulate() const {
    std::vector<std::vector<b2Vec2>> triangles;
	triangles.reserve(m_Points.size() - 2);

    std::vector<Vector2D> polygon = m_Points;

    Vector2D t1 = polygon[1] - polygon[0];
    Vector2D t2 = polygon[2] - polygon[1];

    double winding = Vector2D::Cross(t1, t2);

    if (winding < 0) {
        std::reverse(polygon.begin(), polygon.end());
    }

    while (polygon.size() > 3) {
        std::size_t p1 = 0, p2 = 1, p3 = 2;
        bool earFound = false;

        while (p3 != 1) {
            Vector2D v1 = polygon[p2] - polygon[p1];
            Vector2D v2 = polygon[p3] - polygon[p2];

            double cross = Vector2D::Cross(v1, v2);

            if (cross > 0) {
                bool isEar = true;

                for (std::size_t i = 0; i < polygon.size(); i++) {
                    if (i != p1 && i != p2 && i != p3) {
                        if (Vector2D::Cross(polygon[i] - polygon[p1], polygon[p3] - polygon[p1]) > 0 &&
                            Vector2D::Cross(polygon[i] - polygon[p2], polygon[p1] - polygon[p2]) > 0 &&
                            Vector2D::Cross(polygon[i] - polygon[p3], polygon[p2] - polygon[p3]) > 0) {
                            isEar = false;
                            break;
                        }
                    }
                }

                if (isEar) {
                    triangles.push_back({
						b2Vec2(polygon[p1].x + m_Offset.x, polygon[p1].y + m_Offset.y),
						b2Vec2(polygon[p2].x + m_Offset.x, polygon[p2].y + m_Offset.y),
						b2Vec2(polygon[p3].x + m_Offset.x, polygon[p3].y + m_Offset.y)
					});
                    polygon.erase(polygon.begin() + p2);
                    earFound = true;
                    break;
                }
            }

            p1 = p2;
            p2 = p3;
            p3 = (p3 + 1) % polygon.size();
        }

        if (!earFound) {
            break;
        }
    }

    if (polygon.size() == 3) {
        triangles.push_back({
			b2Vec2(polygon[0].x + m_Offset.x, polygon[0].y + m_Offset.y),
			b2Vec2(polygon[1].x + m_Offset.x, polygon[1].y + m_Offset.y),
			b2Vec2(polygon[2].x + m_Offset.x, polygon[2].y + m_Offset.y)
		});
    }

    return triangles;
}


} // namespace engine2d