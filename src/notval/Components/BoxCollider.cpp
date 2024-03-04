#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) 
	: Collider(gameObj), m_Dimensions(Vector2D::one)
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
		-m_Rotation * M_PI / 180.0
	);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	attachedRigidBody = gameObj->GetComponentInParent<RigidBody>();

	if (attachedRigidBody != nullptr) {
		CreateColliderOnRigidBody(&boxFixture);
	}
	else {
		b2BodyDef boxBody = GetStaticBodyDef();

		CreateStaticCollider(&boxBody, &boxFixture);
	}

	AddFixtureToMap();
}

std::unique_ptr<Component> BoxCollider::Clone() const {
	return std::make_unique<BoxCollider>(*this);
}

void BoxCollider::SetTransform(const Vector2D& dimensions, const Vector2D& offset, const double rotation) {
	RemoveFixtureFromMap();

	m_Dimensions = dimensions;
	m_Offset = offset;
	m_Rotation = rotation;

	b2PolygonShape boxShape;

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	if (attachedRigidBody) {
		boxShape.SetAsBox(
			m_Dimensions.x / 2.0,
			m_Dimensions.y / 2.0,
			b2Vec2(m_Offset.x, m_Offset.y),
			-m_Rotation * M_PI / 180.0
		);

		attachedRigidBody->m_Body->DestroyFixture(m_Fixture);
		m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);
	}
	else {
		DestroyStaticCollider();

		b2PolygonShape boxShape;
		boxShape.SetAsBox(
			m_Dimensions.x / 2.0,
			m_Dimensions.y / 2.0,
			b2Vec2(0, 0),
			-m_Rotation * M_PI / 180.0
		);

		b2BodyDef boxBody = GetStaticBodyDef();

		CreateStaticCollider(&boxBody, &boxFixture);
	}

	AddFixtureToMap();
}

void BoxCollider::AttachRigidBody(RigidBody* rigidBody) {
	if (attachedRigidBody) {
		return;
	}

	RemoveFixtureFromMap();
	DestroyStaticCollider();

	attachedRigidBody = rigidBody;

	b2PolygonShape boxShape;
	boxShape.SetAsBox(
		m_Dimensions.x / 2.0,
		m_Dimensions.y / 2.0,
		b2Vec2(m_Offset.x, m_Offset.y),
		-m_Rotation * M_PI / 180.0
	);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;
	m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);

	AddFixtureToMap();
}

void BoxCollider::DeatachRigidBody() {
	RemoveFixtureFromMap();

	attachedRigidBody = nullptr;

	b2PolygonShape boxShape;
	boxShape.SetAsBox(
		m_Dimensions.x / 2.0,
		m_Dimensions.y / 2.0,
		b2Vec2(0, 0),
		-m_Rotation * M_PI / 180.0
	);

	b2FixtureDef boxFixture;
	boxFixture.shape = &boxShape;
	boxFixture.density = 1.0f;
	boxFixture.friction = 0.3f;

	b2BodyDef boxBody = GetStaticBodyDef();

	CreateStaticCollider(&boxBody, &boxFixture);
	m_CurrentPosition = transform->position;

	AddFixtureToMap();
}