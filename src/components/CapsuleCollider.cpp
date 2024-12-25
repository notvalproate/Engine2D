#include "components/CapsuleCollider.hpp"
#include "GameObject.hpp"

namespace engine2d {

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
	m_Size = size;
	m_Direction = direction;
	m_Offset = offset;

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
		m_UpperSemi->SetDensity(m_Density / 2.0f);
		m_LowerSemi->SetDensity(m_Density / 2.0f);

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
		CreateStaticBody();
		CreateFixturesOnBody(*m_StaticBody);
	}

	UpdateBounds();
}

b2Shape* CapsuleCollider::GetShape(bool) const {
	b2PolygonShape* boxShape = new b2PolygonShape();

	boxShape->SetAsBox(m_Size.x / 2.0, m_Size.y / 2.0);

	return boxShape;
}

void CapsuleCollider::ResetShape() {
	if (m_AttachedRigidBody) {
		m_AttachedRigidBody->m_Body->DestroyFixture(m_Fixture);
		m_AttachedRigidBody->m_Body->DestroyFixture(m_UpperSemi);
		m_AttachedRigidBody->m_Body->DestroyFixture(m_LowerSemi);

		m_Fixture = nullptr;
		m_UpperSemi = nullptr;
		m_LowerSemi = nullptr;

		CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

		m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);
	}
	else {
		DestroyStaticCollider();
		m_UpperSemi = nullptr;
		m_LowerSemi = nullptr;

		CreateStaticBody();
		CreateFixturesOnBody(*m_StaticBody);
	}
}

void CapsuleCollider::AttachRigidBody(RigidBody* rigidBody) {
	if (m_AttachedRigidBody) {
		return;
	}

	m_Material.reset();

	DestroyStaticCollider();
	m_UpperSemi = nullptr;
	m_LowerSemi = nullptr;

	m_AttachedRigidBody = rigidBody;

	CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

	m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);
}

void CapsuleCollider::DeatachRigidBody() {
	m_AttachedRigidBody = nullptr;
	m_Fixture = nullptr;
	m_UpperSemi = nullptr;
	m_LowerSemi = nullptr;
	m_Material.emplace();

	CreateStaticBody();
	CreateFixturesOnBody(*m_StaticBody);

	m_CurrentPosition = transform->position;
}

void CapsuleCollider::CreateFixturesOnBody(b2Body* body) {

	// For the middle box

	b2PolygonShape polygonShape;
	
	polygonShape.SetAsBox(
		m_Size.x / 2.0, 
		m_Size.y / 2.0, 
		b2Vec2(m_Offset.x, m_Offset.y),
		-m_Rotation * M_PI / 180.0
	);

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
	circleTop.m_p = offset + b2Vec2(m_Offset.x, m_Offset.y);

	b2FixtureDef fixtureTop = GetFixtureDef(&circleTop);
	fixtureTop.density /= 2.0f;

	m_UpperSemi = body->CreateFixture(&fixtureTop);

	// For the bottom circle

	b2CircleShape circleBottom;

	circleBottom.m_radius = radius;
	circleBottom.m_p = -offset + b2Vec2(m_Offset.x, m_Offset.y);

	b2FixtureDef fixtureBottom = GetFixtureDef(&circleBottom);
	fixtureBottom.density /= 2.0f;

	m_LowerSemi = body->CreateFixture(&fixtureBottom);
}

} // namespace engine2d