#include "Components.hpp"

RigidBody::RigidBody(GameObject* gameObj) : Component(gameObj), m_Body(nullptr), m_SensorFixture(nullptr) {
	b2BodyDef boxBody;
	boxBody.type = b2_dynamicBody;
	boxBody.position.Set(transform->position.x, transform->position.y);

	m_Body = gameObj->scene->m_PhysicsWorld->CreateBody(&boxBody);

	BoxCollider* collider = gameObj->GetComponent<BoxCollider>();

	if (collider) {
		collider->AttachRigidBody(this);
	}
	else {
		b2PolygonShape boxShape;
		boxShape.SetAsBox(0.01, 0.01);

		b2FixtureDef boxFixture;
		boxFixture.shape = &boxShape;
		boxFixture.density = 1.0f;
		boxFixture.isSensor = true;

		m_SensorFixture = m_Body->CreateFixture(&boxFixture);
	}
}

void RigidBody::SetGravityScale(const double scale) {
	m_Body->SetGravityScale(scale);
}

void RigidBody::FreezeRotation(const bool set) {
	m_Body->SetFixedRotation(set);
}

void RigidBody::Update() {
	transform->position.x = m_Body->GetPosition().x;
	transform->position.y = m_Body->GetPosition().y;
	transform->rotation = -(m_Body->GetAngle() * 180) / M_PI;
}

void RigidBody::OnColliderAttach() {
	m_Body->DestroyFixture(*m_SensorFixture);
	m_SensorFixture.reset();
}