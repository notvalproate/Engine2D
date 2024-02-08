#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) : Behaviour(gameObj), attachedBody(nullptr) {
	GetAttachedBody(gameObject);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(0.5f, 0.5f);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	m_Fixture = body->CreateFixture(&boxFixture);
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::Update() {
	transform->position.x = m_Body->GetPosition().x;
	transform->position.y = m_Body->GetPosition().y;
}

void BoxCollider::GetAttachedBody(GameObject* gameObj) {

}