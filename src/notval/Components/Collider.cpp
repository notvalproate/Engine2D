#include "Components.hpp"

Collider::Collider(GameObject* gameObj) 
	: Behaviour(gameObj), attachedRigidBody(nullptr), m_Fixture(nullptr), m_Offset(0, 0), m_Rotation(0), m_CurrentPosition(gameObj->transform.position), m_StaticBody(nullptr) 
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

void Collider::CreateColliderOnRigidBody(const b2FixtureDef* fixtureDef) {
	m_Fixture = attachedRigidBody->m_Body->CreateFixture(fixtureDef);
	attachedRigidBody->AttachCollider(this);
}

void Collider::CreateStaticCollider(const b2BodyDef* bodyDef, const b2FixtureDef* fixtureDef) {
	m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(bodyDef);
	m_Fixture = (*m_StaticBody)->CreateFixture(fixtureDef);
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