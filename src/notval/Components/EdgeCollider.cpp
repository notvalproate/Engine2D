#include "Components.hpp"

EdgeCollider::EdgeCollider(GameObject* gameObj) : Collider(gameObj), m_Start(Vector2D(-0.5, 0)), m_End(Vector2D(0.5, 0)) {
	
}

EdgeCollider::~EdgeCollider() {
	if (m_AttachedRigidBody) {
		m_AttachedRigidBody->m_Body->DestroyFixture(m_OppositeDirection);
	}
}

std::unique_ptr<Component> EdgeCollider::Clone() const {
	return std::make_unique<EdgeCollider>(*this);
}

void EdgeCollider::SetEdge(const Vector2D& start, const Vector2D& end) {
	m_Start = start;
	m_End = end;

	ResetShape();
}

b2Shape* EdgeCollider::GetShape(bool useOffset) const {
	b2ChainShape* chainShape = new b2ChainShape();
	b2Vec2 offset(0, 0);

	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	b2Vec2 verts[3] = { b2Vec2(m_Start.x, m_Start.y), b2Vec2(m_End.x, m_End.y), b2Vec2(m_End.x + 0.5, m_End.y - 0.5) };

	chainShape->CreateChain(verts, 3, b2Vec2(-1.0f, 0.0f), b2Vec2(1.0f, 0.0f));

	return chainShape;
}

void EdgeCollider::Awake() {

}

void EdgeCollider::ResetShape() {

}

void EdgeCollider::RemoveFixtureFromMap() const {
	auto& sceneColliderMap = gameObject->scene->m_FixtureColliderMap;

	auto it = sceneColliderMap.find(m_Fixture);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}
	
	it = sceneColliderMap.find(m_OppositeDirection);

	if (it != sceneColliderMap.end()) {
		sceneColliderMap.erase(it);
	}
}

void EdgeCollider::AddFixtureToMap() {
	gameObject->scene->m_FixtureColliderMap[m_OppositeDirection] = this;
	gameObject->scene->m_FixtureColliderMap[m_Fixture] = this;
}

void EdgeCollider::AttachRigidBody(RigidBody* rigidBody) {


}
void EdgeCollider::DeatachRigidBody() {

}

void EdgeCollider::CreateFixturesOnBody(b2Body* body) {
	b2ChainShape chainShape;

	b2Vec2 verts[3] = { b2Vec2(m_Start.x, m_Start.y), b2Vec2(m_End.x, m_End.y), b2Vec2(m_End.x + 0.5, m_End.y - 0.5) };
	b2Vec2 vertsrev[3] = { b2Vec2(m_End.x + 0.5, m_End.y - 0.5), b2Vec2(m_End.x, m_End.y), b2Vec2(m_Start.x, m_Start.y) };

	chainShape.CreateChain(verts, 3, b2Vec2(-1.0f, 0.0f), b2Vec2(1.0f, 0.0f));

	b2FixtureDef fixture = GetFixtureDef(&chainShape);

	m_Fixture = body->CreateFixture(&fixture);

	chainShape.CreateChain(vertsrev, 3, b2Vec2(1.0f, 0.0f), b2Vec2(-1.0f, 0.0f));

	b2FixtureDef fixtureRev = GetFixtureDef(&chainShape);

	m_OppositeDirection = body->CreateFixture(&fixtureRev);
}