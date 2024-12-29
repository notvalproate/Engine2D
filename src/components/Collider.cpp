#include "components/Collider.hpp"
#include "GameObject.hpp"

namespace engine2d {

// CIRCLE COLLIDER BOUNCES, BUT NOT WHEN SLOW.
// BOX COLLIDER DOESNT BOUNCE AT ALL
Collider::Collider(GameObject* gameObj)
	: Behaviour(gameObj),
	m_AttachedRigidBody(nullptr),
	m_Fixtures({}),
	m_Offset(0, 0),
	m_Rotation(0),
	m_Material(),
	m_Density(1.0f),
	m_CurrentPosition(gameObj->transform.position), 
	m_StaticBody(nullptr)
{

}

Collider::~Collider() {
	if (m_AttachedRigidBody) {
		for (auto fixture : m_Fixtures) {
			m_AttachedRigidBody->m_Body->DestroyFixture(fixture);
		}
		m_Fixtures.clear();

		auto& rbColliders = m_AttachedRigidBody->m_AttachedColliders;

		for (size_t i = 0; i < rbColliders.size(); i++) {
			if (rbColliders[i] == this) {
				rbColliders.erase(rbColliders.begin() + i);
				break;
			}
		}
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

	if (m_AttachedRigidBody->m_AutoMassEnabled) {
		for (auto fixture : m_Fixtures) {
			fixture->SetDensity(m_Density);
		}
		m_AttachedRigidBody->m_Body->ResetMassData();
	}
}

void Collider::SetMaterial(const PhysicsMaterial& material) {
	m_Material = material;

	ResetShape();
}

void Collider::SetFriction(const double friction) {
	if (m_Material.has_value()) {
		(*m_Material).friction = friction;
		ResetShape();
	}
}

void Collider::SetBounciness(const double bounciness) {
	if (m_Material.has_value()) {
		(*m_Material).bounciness = bounciness;

		for (auto fixture : m_Fixtures) {
			fixture->SetRestitution(bounciness);
		}
	}
}

void Collider::SetAsTrigger(const bool sensor) {
	m_IsTrigger = sensor;

	ResetShape();
}

double Collider::GetDensity() const {
	return m_Density;
}

std::optional<PhysicsMaterial> Collider::GetMaterial() const {
	return m_Material;
}

double Collider::GetFriction() const {
	if (m_Material.has_value()) {
		return (*m_Material).friction;
	}

	if (m_AttachedRigidBody) {
		return m_AttachedRigidBody->m_Material.friction;
	}

	return 0;
}

double Collider::GetBounciness() const {
	if (m_Material.has_value()) {
		return (*m_Material).bounciness;
	}

	if (m_AttachedRigidBody) {
		return m_AttachedRigidBody->m_Material.bounciness;
	}

	return 0;
}

void Collider::Awake() {
	auto shapes = GetShapes(true);

	m_AttachedRigidBody = gameObject->GetComponentInParent<RigidBody>();

	if (m_AttachedRigidBody != nullptr) {
		CreateColliderOnRigidBody(shapes);
		m_AttachedRigidBody->AttachCollider(this);
		UpdateMassData();
	}
	else {
		m_Material.emplace();
		CreateStaticCollider(shapes);
	}

	for (auto shape : shapes) {
		delete shape;
	}

	UpdateBounds();
}

void Collider::Update() {
	for (const auto& contact : m_Contacts) {
		gameObject->OnCollisionStay(Collision(contact.first, this));
	}
	
	for (const auto& overlap : m_Overlaps) {
		gameObject->OnTriggerStay(Collision(overlap.first, this));
	}

	if (m_AttachedRigidBody) {
		UpdateBounds();
		return;
	}

	if (m_CurrentPosition != transform->position) {
		UpdateStaticPosition();
	}
}

void Collider::UpdateBounds() {
	b2AABB boundingBox;
	float max = std::numeric_limits<float>::max();
	boundingBox.lowerBound = b2Vec2(max, max);
	boundingBox.upperBound = b2Vec2(-max, -max);

	for (auto fixture : m_Fixtures) {
		b2AABB fixtureBox = fixture->GetAABB(0);
		boundingBox.Combine(fixtureBox);
	}

	m_BoundingBox.center = boundingBox.GetCenter();
	m_BoundingBox.extents = boundingBox.GetExtents();
	m_BoundingBox.max = m_BoundingBox.center + m_BoundingBox.extents;
	m_BoundingBox.min = m_BoundingBox.center - m_BoundingBox.extents;
	m_BoundingBox.size = m_BoundingBox.extents * 2;
}

void Collider::UpdateStaticPosition() {
	Vector2D newPosition((*m_StaticBody)->GetTransform().p);
	newPosition += transform->position - m_CurrentPosition;
	(*m_StaticBody)->SetTransform(b2Vec2(newPosition.x, newPosition.y), (*m_StaticBody)->GetTransform().q.GetAngle());
	m_CurrentPosition = transform->position;
}

void Collider::ResetShape() {
	std::vector<b2Shape*> shapes;

	if (m_AttachedRigidBody) {
		for (auto fixture : m_Fixtures) {
			m_AttachedRigidBody->m_Body->DestroyFixture(fixture);
		}
		m_Fixtures.clear();

		shapes = GetShapes(true);

		CreateColliderOnRigidBody(shapes);
		UpdateMassData();
	}
	else {
		DestroyStaticCollider();

		shapes = GetShapes();

		CreateStaticCollider(shapes);
	}

	for (auto shape : shapes) {
		delete shape;
	}
}

void Collider::ResetDensity() {
	for (auto fixture : m_Fixtures) {
		fixture->SetDensity(m_Density);
	}
}

void Collider::AttachRigidBody(RigidBody* rigidBody) {
	if (m_AttachedRigidBody) {
		return;
	}

	m_Material.reset();
	DestroyStaticCollider();

	m_AttachedRigidBody = rigidBody;

	auto shapes = GetShapes(true);

	CreateColliderOnRigidBody(shapes);
	UpdateMassData();

	for (auto shape : shapes) {
		delete shape;
	}
}

void Collider::DeatachRigidBody() {
	m_AttachedRigidBody = nullptr;

	auto shapes = GetShapes(false);

	m_Material.emplace();
	CreateStaticCollider(shapes);

	for (auto shape : shapes) {
		delete shape;
	}

	m_CurrentPosition = transform->position;
}

void Collider::CreateColliderOnRigidBody(const std::vector<b2Shape*>& colShape) {
	for (auto shape : colShape) {
		b2FixtureDef fixture = GetFixtureDef(shape);
		m_Fixtures.push_back(m_AttachedRigidBody->m_Body->CreateFixture(&fixture));
	}

	m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);
}

void Collider::CreateStaticBody() {
	b2BodyDef body = GetStaticBodyDef();
	m_StaticBody = gameObject->scene->m_PhysicsWorld.get()->CreateBody(&body);
}

void Collider::CreateStaticCollider(const std::vector<b2Shape*>& colShape) {
	CreateStaticBody();

	for (auto shape : colShape) {
		b2FixtureDef fixture = GetFixtureDef(shape);
		m_Fixtures.push_back((*m_StaticBody)->CreateFixture(&fixture));
	}
}

void Collider::DestroyStaticCollider() {
	gameObject->scene->m_PhysicsWorld.get()->DestroyBody(*m_StaticBody);
	m_StaticBody.reset();
	m_Fixtures.clear();
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
	fixture.userData.pointer = reinterpret_cast<uintptr_t>(this);
	fixture.shape = colShape;
	fixture.density = m_Density;
	fixture.restitutionThreshold = Physics.bouncinessThreshold;
	fixture.isSensor = m_IsTrigger;

	if (m_Material.has_value()) {
		fixture.friction = (*m_Material).friction;
		fixture.restitution = (*m_Material).bounciness;
	}
	else {
		fixture.friction = m_AttachedRigidBody->m_Material.friction;
		fixture.restitution = m_AttachedRigidBody->m_Material.bounciness;
	}

	return fixture;
}

bool Collider::IsCollidingWith(Collider* collider) const {
	if (m_Contacts.count(collider)) {
		return true;
	}

	return false;
}


bool Collider::IsTriggeringWith(Collider* collider) const {
	if (m_Overlaps.count(collider)) {
		return true;
	}

	return false;
}

} // namespace engine2d