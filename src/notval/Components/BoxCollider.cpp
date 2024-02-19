#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) 
	: Behaviour(gameObj), attachedRigidBody(nullptr), m_Fixture(nullptr), m_Dimensions(Vector2D::one), m_Offset({}), m_Rotation(0), m_CurrentPosition(gameObj->transform.position), m_StaticBody(nullptr)
{
	auto spriteRenderer = gameObj->GetComponent<SpriteRenderer>();
	if (spriteRenderer) {
		m_Dimensions.x = spriteRenderer->GetWorldDimensions().x * gameObj->transform.scale.x;
		m_Dimensions.y = spriteRenderer->GetWorldDimensions().y * gameObj->transform.scale.y;
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
	boxFixture.friction = 0.3f;

	attachedRigidBody = gameObj->GetComponentInParent<RigidBody>();

	if (attachedRigidBody != nullptr) {
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

	// IG UNSAFE? DOESNT REMOVE FIXTURE FROM MAP ON DESTRUCTION FOR NOW
	Physics.AddFixtureToMap(m_Fixture, this);
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::SetTransform(const Vector2D dimensions, const Vector2D offset, const double rotation) {
	Physics.RemoveFixtureFromMap(m_Fixture);

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

	Physics.AddFixtureToMap(m_Fixture, this);
}

void BoxCollider::AttachRigidBody(RigidBody* rigidBody) {
	Physics.RemoveFixtureFromMap(m_Fixture);

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

	Physics.AddFixtureToMap(m_Fixture, this);
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