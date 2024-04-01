#include "Core.hpp"

PhysicsHandler::PhysicsHandler() : bouncinessThreshold(1.0f), m_RenderSceneColliders(false) { }

void PhysicsHandler::RenderColliders() const {
	if (!m_RenderSceneColliders) {
		return;
	}

	b2World* currentWorld = Object::SceneManager.GetCurrentScene()->m_PhysicsWorld.get();

	Color green(0, 255, 0);
	Color red(255, 0, 0);

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

						Object::RenderingPipeline.RenderLine(point1, point2, green);
					}
				}
			}
			else if (shape->GetType() == b2Shape::Type::e_circle) {
				b2CircleShape* circle = dynamic_cast<b2CircleShape*>(shape);

				if (circle) {
					Vector2D center(bodyList->GetWorldPoint(circle->m_p));
					Vector2D right(bodyList->GetWorldPoint(b2Vec2(circle->m_p.x, circle->m_p.y + circle->m_radius)));

					Object::RenderingPipeline.RenderCircle(center, circle->m_radius, green);

					Object::RenderingPipeline.RenderLine(center, right, green);
				}
			}
			else if (shape->GetType() == b2Shape::Type::e_edge) {
				b2EdgeShape* edge = dynamic_cast<b2EdgeShape*>(shape);

				if (edge) {
					Vector2D v1(bodyList->GetWorldPoint(edge->m_vertex1));
					Vector2D v2(bodyList->GetWorldPoint(edge->m_vertex2));

					Object::RenderingPipeline.RenderLine(v1, v2, green);
				}
			}
			else if (shape->GetType() == b2Shape::Type::e_chain) {
				b2ChainShape* chain = dynamic_cast<b2ChainShape*>(shape);

				if (chain) {
					for (int i = 0; i < chain->m_count - 1; i++) {
						Vector2D v1(bodyList->GetWorldPoint(chain->m_vertices[i]));
						Vector2D v2(bodyList->GetWorldPoint(chain->m_vertices[i + 1]));

						Object::RenderingPipeline.RenderLine(v1, v2, green);
					}
				}
			}
		}

		Vector2D centerOfMass(bodyList->GetWorldCenter());
		Object::RenderingPipeline.RenderPoint(centerOfMass, 3, red);
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

#include "Components/Components.hpp"

void PhysicsHandler::ContactListener::BeginContact(b2Contact* contact) {
	auto colliderA = reinterpret_cast<Collider*>(contact->GetFixtureA()->GetUserData().pointer);
	auto colliderB = reinterpret_cast<Collider*>(contact->GetFixtureB()->GetUserData().pointer);

	Collision collisionA = GetCollision(colliderB, colliderA);
	Collision collisionB = GetCollision(colliderA, colliderB);

	colliderA->gameObject->OnCollisionEnter(collisionA);
	colliderB->gameObject->OnCollisionEnter(collisionB);
}

void PhysicsHandler::ContactListener::EndContact(b2Contact* contact) {
	auto colliderA = reinterpret_cast<Collider*>(contact->GetFixtureA()->GetUserData().pointer);
	auto colliderB = reinterpret_cast<Collider*>(contact->GetFixtureB()->GetUserData().pointer);

	if (colliderA->gameObject->m_Destroyed || colliderB->gameObject->m_Destroyed) {
		return;
	}

	Collision collisionA = GetCollision(colliderB, colliderA);
	Collision collisionB = GetCollision(colliderA, colliderB);

	colliderA->gameObject->OnCollisionExit(collisionA);
	colliderB->gameObject->OnCollisionExit(collisionB);
}

Collision PhysicsHandler::ContactListener::GetCollision(Collider* collider, Collider* otherCollider) const {
	Collision collision;
	collision.collider = collider;
	collision.otherCollider = otherCollider;

	collision.rigidBody = collider->GetAttachedRigidBody();
	collision.otherRigidBody = otherCollider->GetAttachedRigidBody();

	collision.transform = collider->transform;
	collision.gameObject = collider->gameObject;

	collision.relativeVelocity = Vector2D::zero;
	collision.contactCount = 1;

	return collision;
}

float PhysicsHandler::RayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
	m_Result.hit = true;
	m_Result.point = point;
	m_Result.normal = normal;
	m_Result.fraction = fraction;

	m_FixtureHit = fixture;

	return fraction;
}