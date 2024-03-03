#include "Components.hpp"

BoxCollider::BoxCollider(GameObject* gameObj) 
	: Behaviour(gameObj), attachedRigidBody(nullptr), m_Fixture(nullptr), m_Dimensions(Vector2D::one), m_Offset(0, 0), m_Rotation(0), m_CurrentPosition(gameObj->transform.position), m_StaticBody(nullptr)
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
		m_Fixture = attachedRigidBody->m_Body->CreateFixture(&boxFixture);
		attachedRigidBody->AttachCollider(this);
	}
	else {
		b2BodyDef boxBody;
		boxBody.type = b2_staticBody;
		boxBody.position.Set(transform->position.x, transform->position.y);

		m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&boxBody);
		m_Fixture = (*m_StaticBody)->CreateFixture(&boxFixture);
	}

	AddFixtureToMap();
}

BoxCollider::~BoxCollider() {
	RemoveFixtureFromMap();

	if (attachedRigidBody) {
		auto& rbColliders = attachedRigidBody->m_AttachedColliders;

		for (size_t i = 0; i < rbColliders.size(); i++) {
			if (rbColliders[i] == this) {
				rbColliders.erase(rbColliders.begin() + i);
				break;
			}
		}

		attachedRigidBody->m_Body->DestroyFixture(m_Fixture);
	}
	else {
		gameObject->scene->m_PhysicsWorld.get()->DestroyBody((*m_StaticBody));
	}
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
		b2PolygonShape boxShape;
		boxShape.SetAsBox(
			m_Dimensions.x / 2.0,
			m_Dimensions.y / 2.0,
			b2Vec2(0, 0),
			-m_Rotation * M_PI / 180.0
		);

		gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);

		b2BodyDef boxBody;
		boxBody.type = b2_staticBody;

		Vector2D newPosition = transform->position + m_Offset;
		newPosition.RotateAround(transform->position, transform->rotation);
		boxBody.position.Set(newPosition.x, newPosition.y);
		boxBody.angle = -transform->rotation * M_PI / 180.0;

		m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&boxBody);
		m_Fixture = (*m_StaticBody)->CreateFixture(&boxFixture);
	}

	AddFixtureToMap();
}

void BoxCollider::AttachRigidBody(RigidBody* rigidBody) {
	if (attachedRigidBody) {
		return;
	}

	RemoveFixtureFromMap();

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

	gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);
	m_StaticBody.reset();

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

	b2BodyDef boxBody;
	boxBody.type = b2_staticBody;

	Vector2D newPosition = transform->position + m_Offset;
	newPosition.RotateAround(transform->position, transform->rotation);
	boxBody.position.Set(newPosition.x, newPosition.y);
	boxBody.angle = -transform->rotation * M_PI / 180.0;

	m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&boxBody);
	m_Fixture = (*m_StaticBody)->CreateFixture(&boxFixture);
	m_CurrentPosition = transform->position;

	AddFixtureToMap();
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
	Vector2D newPosition((*m_StaticBody)->GetTransform().p);
	newPosition += transform->position - m_CurrentPosition;
	(*m_StaticBody)->SetTransform(b2Vec2(newPosition.x, newPosition.y), (*m_StaticBody)->GetTransform().q.GetAngle());
	m_CurrentPosition = transform->position;
}

void BoxCollider::RemoveFixtureFromMap() {
	auto& sceneColliderMap = gameObject->scene->m_FixtureColliderMap;

	auto it = sceneColliderMap.find(m_Fixture);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}
}

void BoxCollider::AddFixtureToMap() {
	gameObject->scene->m_FixtureColliderMap[m_Fixture] = this;
}