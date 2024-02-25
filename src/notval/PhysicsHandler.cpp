#include "Core.hpp"

PhysicsHandler::PhysicsHandler() : m_RenderSceneColliders(false) { }

void PhysicsHandler::RenderColliders() const {
	if (!m_RenderSceneColliders) {
		return;
	}

	b2World* currentWorld = Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get();

	for (b2Body* bodyList = currentWorld->GetBodyList(); bodyList; bodyList = bodyList->GetNext()) {

		b2Vec2 center = bodyList->GetWorldCenter();

		Vector2D centerOfMass(center.x, center.y);

		for (b2Fixture* fixtureList = bodyList->GetFixtureList(); fixtureList; fixtureList = fixtureList->GetNext()) {

			b2Shape* shape = fixtureList->GetShape();

			if (shape->GetType() == b2Shape::Type::e_polygon) {
				b2PolygonShape* polygon = dynamic_cast<b2PolygonShape*>(shape);

				if (polygon) {
					for (int i = 0; i < polygon->m_count; i++) {
						int j = (i + 1) % polygon->m_count;

						Vector2D point1(polygon->m_vertices[i].x, polygon->m_vertices[i].y);
						Vector2D point2(polygon->m_vertices[j].x, polygon->m_vertices[j].y);

						point1.x += centerOfMass.x;
						point1.y += centerOfMass.y;
						point2.x += centerOfMass.x;
						point2.y += centerOfMass.y;

						point1.RotateAround(centerOfMass, -(bodyList->GetAngle() * 180) / M_PI);
						point2.RotateAround(centerOfMass, -(bodyList->GetAngle() * 180) / M_PI);

						Object::RenderingPipeline.RenderLine(point1, point2, Color(0, 255, 0));
					}
				}
			}
		}

		Object::RenderingPipeline.RenderPoint(Vector2D(centerOfMass.x, centerOfMass.y), 3, Color(255, 0, 0));
	}
}

void PhysicsHandler::AddFixtureToMap(b2Fixture* fixture, BoxCollider* collider) {
	auto& sceneColliderMap = Object::SceneManager.GetCurrentScene()->m_FixtureColliderMap;

	sceneColliderMap[fixture] = collider;
}

void PhysicsHandler::RemoveFixtureFromMap(b2Fixture* fixture) {
	auto& sceneColliderMap = Object::SceneManager.GetCurrentScene()->m_FixtureColliderMap;

	auto it = sceneColliderMap.find(fixture);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}

}

void PhysicsHandler::SetRenderColliders(const bool set) {
	m_RenderSceneColliders = set;
}

void PhysicsHandler::SetGravity(const Vector2D& gravity) const {
	Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get()->SetGravity(b2Vec2(gravity.x, gravity.y));
}

Vector2D PhysicsHandler::GetGravity() const {
	return Vector2D(Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get()->GetGravity());
}

RayCastHit PhysicsHandler::RayCast(const Vector2D& origin, const Vector2D& direction, float distance) const {
	b2Vec2 originB2 = b2Vec2(origin.x, origin.y);
	b2Vec2 directionB2 = b2Vec2(direction.x, direction.y);
	b2Vec2 endB2 = originB2 + (distance * directionB2);

	b2World* world = Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get();
	auto& sceneColliderMap = Object::SceneManager.GetCurrentScene()->m_FixtureColliderMap;

	RayCastCallback callback;

	world->RayCast(&callback, originB2, endB2);

	if (callback.DidHit()) {
		RayCastHit& result = callback.GetResult(); 
		result.distance = distance * result.fraction;
		auto it = sceneColliderMap.find(callback.GetFixture());

		if (it != sceneColliderMap.end()) {
			result.collider = it->second;
		}

		return result;
	}
	else {
		return RayCastHit();
	}
}

float PhysicsHandler::RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
	m_result.hit = true;
	m_result.point = point;
	m_result.normal = normal;
	m_result.fraction = fraction;

	fixtureHit = fixture;

	return fraction;
}