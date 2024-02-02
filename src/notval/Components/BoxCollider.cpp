#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObject) : Behaviour(gameObject), m_Body(nullptr) {
	b2BodyDef boxBody;
	boxBody.type = b2_dynamicBody;
	boxBody.position.Set(transform->position.x / 10.0f, transform->position.y / 10.0f);
	m_Body = CollisionManager.m_World.get()->CreateBody(&boxBody);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(5.0f, 5.0f);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	m_Fixture = m_Body->CreateFixture(&boxFixture);
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::Update() {
	transform->position.x = m_Body->GetPosition().x * 10.0f;
	transform->position.y = m_Body->GetPosition().y * 10.0f;
}