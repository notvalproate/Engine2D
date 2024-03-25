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

		for (auto& fixture : m_FixtureVector) {
			fixture->SetDensity(m_Density);
		}

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

		for (auto& fixture : m_FixtureVector) {
			m_AttachedRigidBody->m_Body->DestroyFixture(fixture);
		}

		m_Fixture = nullptr;
		m_FixtureVector.clear();

		CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

		m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);
	}
	else {
		gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);
		m_StaticBody.reset();
		m_Fixture = nullptr;
		m_FixtureVector.clear();

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

	for (auto& fixture : m_FixtureVector) {
		auto it = sceneColliderMap.find(fixture);

		if (it != sceneColliderMap.end()) {
			sceneColliderMap.erase(fixture);
		}
	}
}

void CapsuleCollider::AddFixtureToMap() {
	for (auto& fixture : m_FixtureVector) {
		gameObject->scene->m_FixtureColliderMap[fixture] = this;
	}

	gameObject->scene->m_FixtureColliderMap[m_Fixture] = this;
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
	m_FixtureVector.clear();

	m_AttachedRigidBody = rigidBody;

	CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

	m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);

	AddFixtureToMap();
}

void CapsuleCollider::DeatachRigidBody() {
	RemoveFixtureFromMap();

	m_AttachedRigidBody = nullptr;
	m_Fixture = nullptr;
	m_FixtureVector.clear();
	m_Material.emplace();

	b2BodyDef body = GetStaticBodyDef();
	m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&body);

	CreateFixturesOnBody(*m_StaticBody);

	m_CurrentPosition = transform->position;

	AddFixtureToMap();
}

void CapsuleCollider::CreateFixturesOnBody(b2Body* body) {
	for (std::size_t i = 0; i < m_Paths.size(); i++) {
		b2PolygonShape polygonShape;
		polygonShape.Set(&m_Paths[i][0], m_Paths[i].size());

		b2FixtureDef fixture = GetFixtureDef(&polygonShape);

		if (i == 0) {
			m_Fixture = body->CreateFixture(&fixture);
			continue;
		}

		m_FixtureVector.push_back(body->CreateFixture(&fixture));
	}
}