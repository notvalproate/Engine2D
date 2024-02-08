#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) 
	: Behaviour(gameObj), attachedRigidBody(nullptr), m_Fixture(nullptr), m_CurrentPosition(gameObj->transform.position)
{
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
		UpdateFixturePosition();
	}
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::Update() {
	if (m_CurrentPosition != transform->position) {
		UpdateFixturePosition();
	}
}

void BoxCollider::GetAttachedBody(GameObject* gameObj) {
	attachedRigidBody = gameObj->GetComponentInParent<RigidBody>();
}

void BoxCollider::UpdateFixturePosition() {
	b2PolygonShape* polygon = dynamic_cast<b2PolygonShape*>(m_Fixture->GetShape());

	Vector2D distanceMoved = transform->position - m_CurrentPosition;

	for (int i = 0; i < polygon->m_count; i++) {
		polygon->m_vertices[i].x += distanceMoved.x;
		polygon->m_vertices[i].y += distanceMoved.y;
	}
}