#include "PhysicsHandler.hpp"
#include "SceneHandler.hpp"

namespace engine2d {

PhysicsHandler::PhysicsHandler() : bouncinessThreshold(1.0f), m_RenderSceneColliders(false) { }

void PhysicsHandler::RenderColliders() const {
	if (!m_RenderSceneColliders) {
		return;
	}

	b2World* currentWorld = Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get();

	for (b2Body* bodyList = currentWorld->GetBodyList(); bodyList; bodyList = bodyList->GetNext()) {

		for (b2Fixture* fixtureList = bodyList->GetFixtureList(); fixtureList; fixtureList = fixtureList->GetNext()) {

			b2Shape* shape = fixtureList->GetShape();

			if (shape->GetType() == b2Shape::Type::e_polygon) {
				b2PolygonShape* polygon = dynamic_cast<b2PolygonShape*>(shape);

				if (polygon) {
					for (int i = 0; i < polygon->m_count; i++) {
						int j = (i + 1) % polygon->m_count;

						Vector2D point1(bodyList->GetWorldPoint(polygon->m_vertices[i]));
						Vector2D point2(bodyList->GetWorldPoint(polygon->m_vertices[j]));

						Object::RenderingPipeline.RenderLine(point1, point2, Color::green);
					}
				}
			}
			else if (shape->GetType() == b2Shape::Type::e_circle) {
				b2CircleShape* circle = dynamic_cast<b2CircleShape*>(shape);

				if (circle) {
					Vector2D center(bodyList->GetWorldPoint(circle->m_p));
					Vector2D right(bodyList->GetWorldPoint(b2Vec2(circle->m_p.x, circle->m_p.y + circle->m_radius)));

					Object::RenderingPipeline.RenderCircle(center, circle->m_radius, Color::green);

					Object::RenderingPipeline.RenderLine(center, right, Color::green);
				}
			}
			else if (shape->GetType() == b2Shape::Type::e_edge) {
				b2EdgeShape* edge = dynamic_cast<b2EdgeShape*>(shape);

				if (edge) {
					Vector2D v1(bodyList->GetWorldPoint(edge->m_vertex1));
					Vector2D v2(bodyList->GetWorldPoint(edge->m_vertex2));

					Object::RenderingPipeline.RenderLine(v1, v2, Color::green);
				}
			}
			else if (shape->GetType() == b2Shape::Type::e_chain) {
				b2ChainShape* chain = dynamic_cast<b2ChainShape*>(shape);

				if (chain) {
					for (int i = 0; i < chain->m_count - 1; i++) {
						Vector2D v1(bodyList->GetWorldPoint(chain->m_vertices[i]));
						Vector2D v2(bodyList->GetWorldPoint(chain->m_vertices[i + 1]));

						Object::RenderingPipeline.RenderLine(v1, v2, Color::green);
					}
				}
			}
		}

		Vector2D centerOfMass(bodyList->GetWorldCenter());
		Object::RenderingPipeline.RenderPoint(centerOfMass, 3, Color::red);
	}
}

void PhysicsHandler::SetRenderColliders(const bool set) {
	m_RenderSceneColliders = set;
}

void PhysicsHandler::SetGravity(const Vector2D& gravity) const {
	if (Scene* currentScene = Object::SceneManager.GetCurrentScene()) {
		currentScene->m_PhysicsWorld.get()->SetGravity(b2Vec2(gravity.x, gravity.y));
	}
}

Vector2D PhysicsHandler::GetGravity() const {
	if (Scene* currentScene = Object::SceneManager.GetCurrentScene()) {
		return Vector2D(currentScene->m_PhysicsWorld.get()->GetGravity());
	}

	return Vector2D::zero;
}

RayCastHit PhysicsHandler::RayCast(const Vector2D& origin, const Vector2D& direction, float distance) const {
	b2Vec2 originB2 = b2Vec2(origin.x, origin.y);
	b2Vec2 directionB2 = b2Vec2(direction.x, direction.y);
	b2Vec2 endB2 = originB2 + (distance * directionB2);

	b2World* world = Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get();

	RayCastCallback callback;

	world->RayCast(&callback, originB2, endB2);

	if (callback.DidHit()) {
		RayCastHit& result = callback.GetResult();
		result.distance = distance * result.fraction;
		result.collider = reinterpret_cast<Collider*>(callback.GetFixture()->GetUserData().pointer);

		return result;
	}
	else {
		return RayCastHit();
	}
}

float PhysicsHandler::RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
	m_Result.hit = true;
	m_Result.point = point;
	m_Result.normal = normal;
	m_Result.fraction = fraction;

	m_FixtureHit = fixture;

	return fraction;
}

#include "components/Collider.hpp"
#include "GameObject.hpp"

void PhysicsHandler::ContactListener::BeginContact(b2Contact* contact) {
	auto colliderA = reinterpret_cast<Collider*>(contact->GetFixtureA()->GetUserData().pointer);
	auto colliderB = reinterpret_cast<Collider*>(contact->GetFixtureB()->GetUserData().pointer);

	if (colliderA->m_IsTrigger || colliderB->m_IsTrigger) {
		colliderA->m_Overlaps[colliderB]++;
		if (!colliderB->m_Overlaps[colliderA]++) {
			colliderA->gameObject->OnTriggerEnter(Collision(colliderB, colliderA));
			colliderB->gameObject->OnTriggerEnter(Collision(colliderA, colliderB));
		}
	}
	else {
		colliderA->m_Contacts[colliderB]++;
		if (!colliderB->m_Contacts[colliderA]++) {
			colliderA->gameObject->OnCollisionEnter(Collision(colliderB, colliderA));
			colliderB->gameObject->OnCollisionEnter(Collision(colliderA, colliderB));
		}
	}
}

void PhysicsHandler::ContactListener::EndContact(b2Contact* contact) {
	auto colliderA = reinterpret_cast<Collider*>(contact->GetFixtureA()->GetUserData().pointer);
	auto colliderB = reinterpret_cast<Collider*>(contact->GetFixtureB()->GetUserData().pointer);

	if (colliderA->m_IsTrigger || colliderB->m_IsTrigger) {
		colliderA->m_Overlaps[colliderB]--;
		colliderB->m_Overlaps[colliderA]--;

		if (colliderA->m_Overlaps[colliderB] == 0) {
			colliderA->m_Overlaps.erase(colliderB);
			colliderB->m_Overlaps.erase(colliderA);

			colliderA->gameObject->OnTriggerExit(Collision(colliderB, colliderA));
			colliderB->gameObject->OnTriggerExit(Collision(colliderA, colliderB));
		}
	}
	else {
		colliderA->m_Contacts[colliderB]--;
		colliderB->m_Contacts[colliderA]--;

		if (colliderA->m_Contacts[colliderB] == 0) {
			colliderA->m_Contacts.erase(colliderB);
			colliderB->m_Contacts.erase(colliderA);

			colliderA->gameObject->OnCollisionExit(Collision(colliderB, colliderA));
			colliderB->gameObject->OnCollisionExit(Collision(colliderA, colliderB));
		}
	}
}

} // namespace engine2d