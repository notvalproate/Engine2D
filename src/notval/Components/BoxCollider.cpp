#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) : Behaviour(gameObj), attachedRigidBody(nullptr), m_Fixture(nullptr) {
	GetAttachedBody(gameObj);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(0.5f, 0.5f);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	if (attachedRigidBody) {
		m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);
	}
	else {
		m_Fixture = gameObj->scene->m_RootStaticBody->CreateFixture(&boxFixture);
	}
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::GetAttachedBody(GameObject* gameObj) {
	attachedRigidBody = gameObj->GetComponentInParent<RigidBody>();
}