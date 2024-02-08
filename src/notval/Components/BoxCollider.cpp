#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) 
	: Behaviour(gameObj), attachedRigidBody(nullptr), m_Fixture(nullptr), m_CurrentPosition(gameObj->transform.position), m_StaticBody(nullptr)
{
	attachedRigidBody = gameObj->GetComponentInParent<RigidBody>();

	b2PolygonShape boxShape;
	boxShape.SetAsBox(0.2, 0.2);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	if (attachedRigidBody) {
		m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);
	}
	else {
		b2BodyDef boxBody;
		boxBody.type = b2_staticBody;
		boxBody.position.Set(transform->position.x, transform->position.y);

		m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&boxBody);
		m_Fixture = (*m_StaticBody)->CreateFixture(&boxFixture);
	}
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::Update() {
	if (attachedRigidBody) {
		return;
	}

	if (m_CurrentPosition != transform->position) {
		UpdateStaticPosition();
	}
}

void BoxCollider::UpdateStaticPosition() {
	m_CurrentPosition = transform->position;
	(*m_StaticBody)->SetTransform(b2Vec2(m_CurrentPosition.x, m_CurrentPosition.y), 0);
}