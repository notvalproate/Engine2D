#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) 
	: Behaviour(gameObj), attachedRigidBody(nullptr), m_Fixture(nullptr), m_Dimensions(Vector2D::one), m_Offset({}), m_Rotation(0), m_CurrentPosition(gameObj->transform.position), m_StaticBody(nullptr)
{
	auto spriteRenderer = gameObj->GetComponent<SpriteRenderer>();
	if (spriteRenderer) {
		m_Dimensions = spriteRenderer->GetWorldDimensions();
		m_Rotation = gameObj->transform.rotation;
	}

	b2PolygonShape boxShape;
	boxShape.SetAsBox(
		m_Dimensions.x / 2.0,
		m_Dimensions.y / 2.0,
		b2Vec2(m_Offset.x, m_Offset.y),
		(m_Rotation * M_PI) / 180.0
	);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.0f;

	attachedRigidBody = gameObj->GetComponentInParent<RigidBody>();

	if (attachedRigidBody) {
		m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);
		attachedRigidBody->OnColliderAttach();
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

void BoxCollider::SetTransform(const Vector2D dimensions, const Vector2D offset, const double rotation) {
	m_Dimensions = dimensions;
	m_Offset = offset;
	m_Rotation = 0;

	b2PolygonShape boxShape;
	boxShape.SetAsBox(
		m_Dimensions.x / 2.0,
		m_Dimensions.y / 2.0,
		b2Vec2(m_Offset.x, m_Offset.y),
		(m_Rotation * M_PI) / 180.0
	);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	if (attachedRigidBody) {
		attachedRigidBody->m_Body->DestroyFixture(m_Fixture);
		m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);
	}
	else {
		(*m_StaticBody)->DestroyFixture(m_Fixture);
		m_Fixture = (*m_StaticBody)->CreateFixture(&boxFixture);
	}
}

void BoxCollider::AttachRigidBody(RigidBody* rigidBody) {
	attachedRigidBody = rigidBody;

	b2PolygonShape boxShape;
	boxShape.SetAsBox(
		m_Dimensions.x / 2.0,
		m_Dimensions.y / 2.0,
		b2Vec2(m_Offset.x, m_Offset.y),
		(m_Rotation * M_PI) / 180.0
	);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;
	m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);

	gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);
	m_StaticBody.reset();
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