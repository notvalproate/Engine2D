#include "Components.hpp"

CapsuleCollider::CapsuleCollider(GameObject* gameObj)
	: Collider(gameObj)
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

b2Shape* CapsuleCollider::GetShape(bool useOffset) const {
	b2PolygonShape* boxShape = new b2PolygonShape();


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
	b2PolygonShape polygonShape;

	b2FixtureDef fixture = GetFixtureDef(&polygonShape);

	m_Fixture = body->CreateFixture(&fixture);
}