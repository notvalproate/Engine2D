#include "Components.hpp"

RigidBody::RigidBody(GameObject* gameObj) : Component(gameObj), drag(0.0), angularDrag(0.0), m_Body(nullptr), m_SensorFixture(nullptr) {
	b2BodyDef boxBody;
	boxBody.type = b2_dynamicBody;
	boxBody.position.Set(transform->position.x, transform->position.y);

	m_Body = gameObj->scene->m_PhysicsWorld->CreateBody(&boxBody);

	BoxCollider* collider = gameObj->GetComponent<BoxCollider>();

	if (collider) {
		collider->AttachRigidBody(this);
	}
	else {
		b2PolygonShape boxShape;
		boxShape.SetAsBox(0.01, 0.01);

		b2FixtureDef boxFixture;
		boxFixture.shape = &boxShape;
		boxFixture.density = 1.0f;
		boxFixture.isSensor = true;

		m_SensorFixture = m_Body->CreateFixture(&boxFixture);
	}
}

void RigidBody::AddForce(const Vector2D force) {
	m_Body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
}

void RigidBody::AddTorque(const double force) {
	m_Body->ApplyTorque(force, true);
}

void RigidBody::SetMass(const float mass) {
	float currentMass = m_Body->GetMass();
	float density = (currentMass > 0.0) ? mass / currentMass : 0.0;

	for (b2Fixture* fixture = m_Body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetDensity(density);
		m_Body->ResetMassData();
	}
}

void RigidBody::SetVelocity(const Vector2D vel) {
	m_Body->SetLinearVelocity(b2Vec2(vel.x, vel.y));
}

void RigidBody::SetAngularVelocity(const float vel) {
	m_Body->SetAngularVelocity(vel);
}

void RigidBody::SetBodyType(const RigidBodyType type) {
	m_Body->SetType((b2BodyType)type);
}

void RigidBody::SetGravityScale(const double scale) {
	m_Body->SetGravityScale(scale);
}

void RigidBody::FreezeRotation(const bool set) {
	m_Body->SetFixedRotation(set);
}

float RigidBody::GetMass() const {
	return m_Body->GetMass();
}

Vector2D RigidBody::GetVelocity() const {
	return Vector2D(m_Body->GetLinearVelocity());
}

float RigidBody::GetAngularVelocity() const {
	return m_Body->GetAngularVelocity();
}

RigidBodyType RigidBody::GetBodyType() const {
	return (RigidBodyType)m_Body->GetType();
}

Vector2D RigidBody::GetCentreOfMass() const {
	return Vector2D(m_Body->GetWorldCenter());
}

void RigidBody::Update() {
	transform->position.x = m_Body->GetPosition().x;
	transform->position.y = m_Body->GetPosition().y;
	transform->rotation = -(m_Body->GetAngle() * 180) / M_PI;

	if (GetBodyType() != RigidBodyType::Dynamic) {
		return;
	}

	m_Body->ApplyForceToCenter(b2Vec2(0, Physics.gravity), true);

	Vector2D velocity(m_Body->GetLinearVelocity());

	if (velocity != Vector2D::zero) {
		double magSquare = velocity.GetMagnitudeSquared();
		velocity.Normalize();
		Vector2D dragForce = -drag * magSquare * velocity;
		AddForce(dragForce);
	}

	float angularVelocity(m_Body->GetAngularVelocity());

	if (std::abs(angularVelocity) > 0.001) {
		AddTorque(-angularDrag * angularVelocity);
	}
}

void RigidBody::OnColliderAttach() {
	m_Body->DestroyFixture(*m_SensorFixture);
	m_SensorFixture.reset();
}