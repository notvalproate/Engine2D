#include "Components.hpp"

// CIRCLE COLLIDER BOUNCES, BUT NOT WHEN SLOW.
// BOX COLLIDER DOESNT BOUNCE AT ALL
Collider::Collider(GameObject* gameObj)
	: Behaviour(gameObj),
	attachedRigidBody(nullptr),
	m_Fixture(nullptr),
	m_Offset(0, 0),
	m_Rotation(0),
	m_Density(1.0f),
	m_Friction(0.3f),
	m_Bounciness(0),
	m_BouncinessThreshold(0.2),
	m_CurrentPosition(gameObj->transform.position), 
	m_StaticBody(nullptr)
{
	
}

Collider::~Collider() {
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

void Collider::SetDensity(const double density) {
	if (density <= 0) {
		m_Density = 1.0f;
	}
	else {
		m_Density = density;
	}

	if (attachedRigidBody->m_AutoMassEnabled) {
		m_Fixture->SetDensity(m_Density);
		attachedRigidBody->m_Body->ResetMassData();
	}
}

void Collider::SetFriction(const double friction) {
	m_Friction = friction;
	m_Fixture->SetFriction(friction);
}

void Collider::SetBounciness(const double bounciness) {
	m_Bounciness = bounciness;
	m_Fixture->SetRestitution(bounciness);
}

void Collider::SetBouncinessThreshold(const double threshold) {
	m_BouncinessThreshold = threshold;
	m_Fixture->SetRestitutionThreshold(threshold);
}

double Collider::GetDensity() const {
	return m_Density;
}

double Collider::GetFriction(const double bounciness) const {
	return m_Friction;
}

double Collider::GetBounciness() const {
	return m_Bounciness;
}

double Collider::GetBouncinessThreshold() const {
	return m_BouncinessThreshold;
}

void Collider::Awake() {
	b2Shape* boxShape = GetShape(true);

	attachedRigidBody = gameObject->GetComponentInParent<RigidBody>();

	if (attachedRigidBody != nullptr) {
		CreateColliderOnRigidBody(boxShape);
		attachedRigidBody->AttachCollider(this);
	}
	else {
		CreateStaticCollider(boxShape);
	}

	delete boxShape;

	AddFixtureToMap();
}

void Collider::Update() {
	if (attachedRigidBody) {
		return;
	}

	if (m_CurrentPosition != transform->position) {
		UpdateStaticPosition();
	}
}

void Collider::UpdateStaticPosition() {
	Vector2D newPosition((*m_StaticBody)->GetTransform().p);
	newPosition += transform->position - m_CurrentPosition;
	(*m_StaticBody)->SetTransform(b2Vec2(newPosition.x, newPosition.y), (*m_StaticBody)->GetTransform().q.GetAngle());
	m_CurrentPosition = transform->position;
}

void Collider::ResetShape() {
	RemoveFixtureFromMap();

	if (attachedRigidBody) {
		attachedRigidBody->m_Body->DestroyFixture(m_Fixture);

		b2Shape* boxShape = GetShape(true);

		CreateColliderOnRigidBody(boxShape);

		delete boxShape;
	}
	else {
		DestroyStaticCollider();

		b2Shape* boxShape = GetShape();

		CreateStaticCollider(boxShape);

		delete boxShape;
	}

	AddFixtureToMap();
}

void Collider::ResetDensity() {
	m_Fixture->SetDensity(m_Density);
}

void Collider::RemoveFixtureFromMap() const {
	auto& sceneColliderMap = gameObject->scene->m_FixtureColliderMap;

	auto it = sceneColliderMap.find(m_Fixture);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}
}

void Collider::AddFixtureToMap() {
	gameObject->scene->m_FixtureColliderMap[m_Fixture] = this;
}

void Collider::AttachRigidBody(RigidBody* rigidBody) {
	if (attachedRigidBody) {
		return;
	}

	RemoveFixtureFromMap();
	DestroyStaticCollider();

	attachedRigidBody = rigidBody;

	b2Shape* shape = GetShape(true);

	CreateColliderOnRigidBody(shape);

	delete shape;

	attachedRigidBody->AttachCollider(this);
	AddFixtureToMap();
}

void Collider::DeatachRigidBody() {
	RemoveFixtureFromMap();

	attachedRigidBody = nullptr;

	b2Shape* shape = GetShape(false);

	CreateStaticCollider(shape);

	delete shape;

	m_CurrentPosition = transform->position;

	AddFixtureToMap();
}

void Collider::CreateColliderOnRigidBody(const b2Shape* colShape) {
	b2FixtureDef fixture = GetFixtureDef(colShape);

	m_Fixture = attachedRigidBody->m_Body->CreateFixture(&fixture);
	attachedRigidBody->SetMass(attachedRigidBody->m_Mass);
}

void Collider::CreateStaticCollider(const b2Shape* colShape) {
	b2BodyDef body = GetStaticBodyDef();
	b2FixtureDef fixture = GetFixtureDef(colShape);

	m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&body);
	m_Fixture = (*m_StaticBody)->CreateFixture(&fixture);
}

void Collider::DestroyStaticCollider() {
	gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);
	m_StaticBody.reset();
	m_Fixture = nullptr;
}

b2BodyDef Collider::GetStaticBodyDef() const {
	Vector2D newPosition = transform->position + m_Offset;
	newPosition.RotateAround(transform->position, transform->rotation);

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position = b2Vec2(newPosition.x, newPosition.y);
	body.angle = -transform->rotation * M_PI / 180.0;

	return body;
}

b2FixtureDef Collider::GetFixtureDef(const b2Shape* colShape) const {
	b2FixtureDef fixture;
	fixture.shape = colShape;
	fixture.density = m_Density;
	fixture.friction = m_Friction;
	fixture.restitutionThreshold = m_BouncinessThreshold;
	fixture.restitution = m_Bounciness;

	return fixture;
}