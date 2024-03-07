#include "Components.hpp"

RigidBody::RigidBody(GameObject* gameObj) : Component(gameObj), drag(0.0), angularDrag(0.0), totalForce(0.0, 0.0), totalTorque(0.0), m_Body(nullptr), m_Mass(1), m_AutoMassEnabled(false), m_AttachedColliders({}), m_SensorFixture(nullptr) {
	b2BodyDef boxBody;
	boxBody.type = b2_dynamicBody;
	boxBody.position.Set(transform->position.x, transform->position.y);
	boxBody.angle = -transform->rotation * M_PI / 180.0;

	m_Body = gameObj->scene->m_PhysicsWorld->CreateBody(&boxBody);

	m_AttachedColliders = gameObj->GetComponents<Collider>();

	if (m_AttachedColliders.size()) {
		for (auto& collider : m_AttachedColliders) {
			collider->AttachRigidBody(this);
		}
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

RigidBody::~RigidBody() {
	gameObject->scene->m_PhysicsWorld.get()->DestroyBody(m_Body);

	for (auto& col : m_AttachedColliders) {
		col->DeatachRigidBody();
	}

	m_AttachedColliders.clear();
}

void RigidBody::AddForce(const Vector2D& force) {
	totalForce += force;
}

void RigidBody::AddForceAtPosition(const Vector2D& force, const Vector2D& position) {
	AddForce(force);
	AddTorque(Vector2D::Cross(position - GetCentreOfMass(), force));
}

void RigidBody::AddRelativeForce(Vector2D force) {
	force.Rotate(transform->rotation);
	AddForce(force);
}

void RigidBody::AddTorque(const double force) {
	totalTorque += force;
}

void RigidBody::SetMass(const float mass) {
	if (m_AutoMassEnabled) {
		return;
	}

	m_Mass = mass;

	float totalArea = 0;

	// TRY TO CACHE THE TOTAL AREA SO ITS FASTER TO SET TRANSFORM AND SET MASS EVERY FRAME

	for (b2Fixture* fixture = m_Body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		b2MassData massData;
		m_Body->GetMassData(&massData);
		fixture->GetMassData(&massData);
		totalArea += massData.mass / fixture->GetDensity();
	}

	float newDensity = (totalArea == 0 ? 1 : mass / totalArea); 

	for (b2Fixture* fixture = m_Body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
		fixture->SetDensity(newDensity);
	}

	m_Body->ResetMassData();
}

void RigidBody::SetAutoMass(const bool use) {
	if (use && !m_AutoMassEnabled) {
		for (auto& collider : m_AttachedColliders) {
			collider->ResetDensity();
		}
		m_Body->ResetMassData();
		m_AutoMassEnabled = true;
		return;
	}

	if (!use && m_AutoMassEnabled){
		SetMass(m_Mass);
		m_AutoMassEnabled = false;
	}
}

void RigidBody::SetVelocity(const Vector2D& vel) {
	m_Body->SetLinearVelocity(b2Vec2(vel.x, vel.y));
}

void RigidBody::SetAngularVelocity(const float vel) {
	m_Body->SetAngularVelocity(-vel * M_PI / 180.0);
}

void RigidBody::SetPosition(const Vector2D& pos) {
	m_Body->SetTransform(b2Vec2(pos.x, pos.y), m_Body->GetAngle());
}

void RigidBody::SetRotation(const double rotation) {
	m_Body->SetTransform(m_Body->GetPosition(), -rotation * M_PI / 180.0);
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

void RigidBody::WakeUp() {
	m_Body->SetAwake(true);
}

void RigidBody::Sleep() {
	m_Body->SetAwake(false);
}

float RigidBody::GetMass() const {
	return m_Mass;
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

std::vector<Collider*> RigidBody::GetAttachedColliders() const {
	return m_AttachedColliders;
}

bool RigidBody::IsAwake() const {
	return m_Body->IsAwake();
}

bool RigidBody::IsSleeping() const {
	return !m_Body->IsAwake();
}

void RigidBody::Update() {
	transform->position.x = m_Body->GetPosition().x;
	transform->position.y = m_Body->GetPosition().y;
	transform->rotation = -(m_Body->GetAngle() * 180) / M_PI;

	if (GetBodyType() != RigidBodyType::Dynamic) {
		return;
	}

	AddDrag();
	AddAngularDrag();

	ApplyTotalForces();
}

void RigidBody::AddDrag() {
	Vector2D velocity(m_Body->GetLinearVelocity());

	if (velocity == Vector2D::zero) {
		return;
	}

	double magSquare = velocity.GetMagnitudeSquared();
	velocity.Normalize();
	Vector2D dragForce = -drag * magSquare * velocity;
	AddForce(dragForce);
}

void RigidBody::AddAngularDrag() {
	float angularVelocity(m_Body->GetAngularVelocity());
	AddTorque(-angularDrag * angularVelocity);
}

void RigidBody::ApplyTotalForces() {
	if (totalForce == Vector2D::zero && totalTorque == 0) {
		return;
	}

	m_Body->ApplyForceToCenter(b2Vec2(totalForce.x, totalForce.y), true);
	m_Body->ApplyTorque(totalTorque, true);

	totalForce.x = 0;
	totalForce.y = 0;
	totalTorque = 0;
}

void RigidBody::AttachCollider(Collider* collider) {
	if (m_SensorFixture.has_value()) {
		m_Body->DestroyFixture(*m_SensorFixture);
		m_SensorFixture.reset();
	}

	m_AttachedColliders.push_back(collider);
}