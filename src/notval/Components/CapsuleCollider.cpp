#include "Components.hpp"

CapsuleCollider::CapsuleCollider(GameObject* gameObj)
	: Collider(gameObj),
	m_Size(1, 1),
	m_Direction(CapsuleDirection::Vertical),
	m_UpperSemi(nullptr),
	m_LowerSemi(nullptr)
{

}

CapsuleCollider::~CapsuleCollider() {
	if (m_AttachedRigidBody) {
		m_AttachedRigidBody->m_Body->DestroyFixture(m_UpperSemi);
		m_AttachedRigidBody->m_Body->DestroyFixture(m_LowerSemi);
	}
}

std::unique_ptr<Component> CapsuleCollider::Clone() const {
	return std::make_unique<CapsuleCollider>(*this);
}

void CapsuleCollider::SetTransform(const Vector2D& size, const CapsuleDirection direction, const Vector2D& offset) {
	ResetShape();
}

void CapsuleCollider::SetDensity(const double density) {
	if (density <= 0) {
		m_Density = 1.0f;
	}
	else {
		m_Density = density;
	}

	if (m_AttachedRigidBody->m_AutoMassEnabled) {
		m_Fixture->SetDensity(m_Density);
		m_UpperSemi->SetDensity(m_Density);
		m_LowerSemi->SetDensity(m_Density);

		m_AttachedRigidBody->m_Body->ResetMassData();
	}
}

void CapsuleCollider::Awake() {
	m_AttachedRigidBody = gameObject->GetComponentInParent<RigidBody>();

	if (m_AttachedRigidBody != nullptr) {
		CreateFixturesOnBody(m_AttachedRigidBody->m_Body);
		m_AttachedRigidBody->AttachCollider(this);
	}
	else {
		b2BodyDef body = GetStaticBodyDef();
		m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&body);

		CreateFixturesOnBody(*m_StaticBody);
	}

	UpdateBounds();
	AddFixtureToMap();
}

b2Shape* CapsuleCollider::GetShape(bool useOffset) const {
	b2PolygonShape* boxShape = new b2PolygonShape();

	boxShape->SetAsBox(m_Size.x / 2.0, m_Size.y / 2.0);

	return boxShape;
}

void CapsuleCollider::ResetShape() {
	RemoveFixtureFromMap();

	if (m_AttachedRigidBody) {
		m_AttachedRigidBody->m_Body->DestroyFixture(m_Fixture);

		m_Fixture = nullptr;
		m_UpperSemi = nullptr;
		m_LowerSemi = nullptr;

		CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

		m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);
	}
	else {
		gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);
		m_StaticBody.reset();
		m_Fixture = nullptr;
		m_UpperSemi = nullptr;
		m_LowerSemi = nullptr;

		b2BodyDef body = GetStaticBodyDef();
		m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&body);

		CreateFixturesOnBody(*m_StaticBody);
	}

	AddFixtureToMap();
}

void CapsuleCollider::RemoveFixtureFromMap() const {
	auto& sceneColliderMap = gameObject->scene->m_FixtureColliderMap;

	auto it = sceneColliderMap.find(m_Fixture);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}
	
	it = sceneColliderMap.find(m_UpperSemi);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}
	
	it = sceneColliderMap.find(m_LowerSemi);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}
}

void CapsuleCollider::AddFixtureToMap() {
	gameObject->scene->m_FixtureColliderMap[m_Fixture] = this;
	gameObject->scene->m_FixtureColliderMap[m_UpperSemi] = this;
	gameObject->scene->m_FixtureColliderMap[m_LowerSemi] = this;
}

void CapsuleCollider::AttachRigidBody(RigidBody* rigidBody) {
	if (m_AttachedRigidBody) {
		return;
	}

	m_Material.reset();
	RemoveFixtureFromMap();

	gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);
	m_StaticBody.reset();
	m_Fixture = nullptr;
	m_UpperSemi = nullptr;
	m_LowerSemi = nullptr;

	m_AttachedRigidBody = rigidBody;

	CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

	m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);

	AddFixtureToMap();
}

void CapsuleCollider::DeatachRigidBody() {
	RemoveFixtureFromMap();

	m_AttachedRigidBody = nullptr;
	m_Fixture = nullptr;
	m_UpperSemi = nullptr;
	m_LowerSemi = nullptr;
	m_Material.emplace();

	b2BodyDef body = GetStaticBodyDef();
	m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&body);

	CreateFixturesOnBody(*m_StaticBody);

	m_CurrentPosition = transform->position;

	AddFixtureToMap();
}

void CapsuleCollider::CreateFixturesOnBody(b2Body* body) {

	// For the middle box

	b2PolygonShape polygonShape;
	
	polygonShape.SetAsBox(m_Size.x / 2.0, m_Size.y / 2.0);

	b2FixtureDef fixture = GetFixtureDef(&polygonShape);

	m_Fixture = body->CreateFixture(&fixture);

	// Radius and offset for circles

	float radius = m_Size.x / 2.0;
	b2Vec2 offset(0, m_Size.y / 2.0);

	if (m_Direction == CapsuleDirection::Horizontal) {
		radius = m_Size.y / 2.0;
		offset = b2Vec2(m_Size.x / 2.0, 0);
	}

	// For the top circle

	b2CircleShape circleTop;

	circleTop.m_radius = radius;
	circleTop.m_p = offset;

	b2FixtureDef fixtureTop = GetFixtureDef(&circleTop);

	m_UpperSemi = body->CreateFixture(&fixtureTop);

	// For the bottom circle

	b2CircleShape circleBottom;

	circleBottom.m_radius = radius;
	circleBottom.m_p = -offset;

	b2FixtureDef fixtureBottom = GetFixtureDef(&circleBottom);

	m_LowerSemi = body->CreateFixture(&fixtureBottom);
}