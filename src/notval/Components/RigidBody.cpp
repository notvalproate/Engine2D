#include "Components.hpp"

RigidBody::RigidBody(GameObject* gameObj) : Behaviour(gameObj), m_Body(nullptr) {
	b2BodyDef boxBody;
	boxBody.type = b2_dynamicBody;
	boxBody.position.Set(transform->position.x, transform->position.y);

	m_Body = gameObject->scene->m_PhysicsWorld->CreateBody(&boxBody);
}