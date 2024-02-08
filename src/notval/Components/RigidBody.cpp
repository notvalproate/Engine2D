#include "Components.hpp"

RigidBody::RigidBody(GameObject* gameObj) : Component(gameObj), m_Body(nullptr) {
	b2BodyDef boxBody;
	boxBody.type = b2_dynamicBody;
	boxBody.position.Set(transform->position.x, transform->position.y);

	m_Body = gameObj->scene->m_PhysicsWorld->CreateBody(&boxBody);

	BoxCollider* collider = gameObj->GetComponent<BoxCollider>();

	if (collider) {
		collider->AttachRigidBody(this);
	}
}

void RigidBody::Update() {
	transform->position.x = m_Body->GetPosition().x;
	transform->position.y = m_Body->GetPosition().y;
	transform->rotation = -(m_Body->GetAngle() * 180) / M_PI;
}