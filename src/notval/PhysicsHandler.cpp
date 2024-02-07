#include "Core.hpp"

PhysicsHandler::PhysicsHandler() : m_RenderSceneColliders(false) { }

void PhysicsHandler::RenderColliders() const {
	if (!m_RenderSceneColliders) {
		return;
	}

	b2World* currentWorld = Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get();

	for (b2Body* bodyList = currentWorld->GetBodyList(); bodyList; bodyList = bodyList->GetNext()) {

		b2Vec2 position = bodyList->GetWorldPoint(b2Vec2(0.0f, 0.0f));

		for (b2Fixture* fixtureList = bodyList->GetFixtureList(); fixtureList; fixtureList = fixtureList->GetNext()) {

			b2Shape* shape = fixtureList->GetShape();

			if (shape->GetType() == b2Shape::Type::e_polygon) {
				b2PolygonShape* polygon = dynamic_cast<b2PolygonShape*>(shape);

				if (polygon) {
					for (int i = 0; i < polygon->m_count; i++) {
						int j = (i + 1) % polygon->m_count;

						Vector2D point1(polygon->m_vertices[i].x, polygon->m_vertices[i].y);
						Vector2D point2(polygon->m_vertices[j].x, polygon->m_vertices[j].y); 

						//Object::RenderingPipeline.RenderLine(point1, point2, Color(255, 0, 0));
					}
				}
			}
		}

	}
}

void PhysicsHandler::SetRenderColliders(const bool set) {
	m_RenderSceneColliders = set;
}