#include "components/Components.hpp"
#include "GameObject.hpp"

EdgeCollider::EdgeCollider(GameObject* gameObj) 
	: Collider(gameObj), 
	m_Points( { Vector2D(-0.5, 0), Vector2D(0.5, 0) } ), 
	m_ReverseFixture(nullptr) 
{
	
}

EdgeCollider::~EdgeCollider() {
	if (m_AttachedRigidBody) {
		m_AttachedRigidBody->m_Body->DestroyFixture(m_ReverseFixture);
	}
}

std::unique_ptr<Component> EdgeCollider::Clone() const {
	return std::make_unique<EdgeCollider>(*this);
}

void EdgeCollider::SetPoints(const std::vector<Vector2D>& points, const Vector2D& offset) {
	m_Offset = offset;
	m_Points = points;

	ResetShape();
}

void EdgeCollider::Reset() {
	SetPoints({ Vector2D(-0.5, 0), Vector2D(0, 0.5) }, Vector2D::zero);
}

b2Shape* EdgeCollider::GetShape(bool useOffset) const {
	b2ChainShape* chainShape = new b2ChainShape();
	b2Vec2 offset(0, 0);

	if (useOffset) {
		offset.Set(m_Offset.x, m_Offset.y);
	}

	b2Vec2 verts[2] = { b2Vec2(0, 0), b2Vec2(1, 1) };

	chainShape->CreateChain(verts, 2, b2Vec2(-1.0f, 0.0f), b2Vec2(1.0f, 0.0f));

	return chainShape;
}

void EdgeCollider::Awake() {
	m_AttachedRigidBody = gameObject->GetComponentInParent<RigidBody>();

	if (m_AttachedRigidBody != nullptr) {
		CreateFixturesOnBody(m_AttachedRigidBody->m_Body);
		m_AttachedRigidBody->AttachCollider(this);
		UpdateMassData();
	}
	else {
		CreateStaticBody();
		CreateFixturesOnBody(*m_StaticBody);
	}

	UpdateBounds();
}

void EdgeCollider::ResetShape() {
	if (m_AttachedRigidBody) {
		m_AttachedRigidBody->m_Body->DestroyFixture(m_Fixture);
		m_AttachedRigidBody->m_Body->DestroyFixture(m_ReverseFixture);

		m_Fixture = nullptr;
		m_ReverseFixture = nullptr;

		CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

		m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);
		UpdateMassData();
	}
	else {
		DestroyStaticCollider();
		m_ReverseFixture = nullptr;

		CreateStaticBody();
		CreateFixturesOnBody(*m_StaticBody);
	}
}

void EdgeCollider::AttachRigidBody(RigidBody* rigidBody) {
	if (m_AttachedRigidBody) {
		return;
	}

	m_Material.reset();

	DestroyStaticCollider();
	m_ReverseFixture = nullptr;

	m_AttachedRigidBody = rigidBody;

	CreateFixturesOnBody(m_AttachedRigidBody->m_Body);

	m_AttachedRigidBody->SetMass(m_AttachedRigidBody->m_Mass);
	UpdateMassData();
}

void EdgeCollider::DeatachRigidBody() {
	m_AttachedRigidBody = nullptr;
	m_Fixture = nullptr;
	m_ReverseFixture = nullptr;
	m_Material.emplace();

	CreateStaticBody();
	CreateFixturesOnBody(*m_StaticBody);

	m_CurrentPosition = transform->position;
}

void EdgeCollider::CreateFixturesOnBody(b2Body* body) {
	std::vector<b2Vec2> forwardPoints;
	forwardPoints.reserve(m_Points.size());

	for (const auto& point : m_Points) {
		forwardPoints.push_back(b2Vec2(point.x + m_Offset.x, point.y + m_Offset.y));
	}

	std::vector<b2Vec2> reversePoints(forwardPoints);
	std::reverse(reversePoints.begin(), reversePoints.end());

	b2ChainShape forwardChain;
	forwardChain.CreateChain(&forwardPoints[0], forwardPoints.size(), forwardPoints.front(), forwardPoints.back());
	b2FixtureDef forwardFixture = GetFixtureDef(&forwardChain);
	m_Fixture = body->CreateFixture(&forwardFixture);

	b2ChainShape reverseChain;
	reverseChain.CreateChain(&reversePoints[0], reversePoints.size(), reversePoints.front(), reversePoints.back());
	b2FixtureDef reverseFixture = GetFixtureDef(&reverseChain);
	m_ReverseFixture = body->CreateFixture(&reverseFixture);
}

void EdgeCollider::UpdateMassData() const {
	if (!m_AttachedRigidBody) {
		return;
	}

	b2MassData massdata;

	massdata.mass = m_AttachedRigidBody->GetMass();
	massdata.center = m_AttachedRigidBody->m_Body->GetLocalCenter();
	massdata.I = m_AttachedRigidBody->m_Body->GetInertia();

	if (massdata.I == 0) {
		massdata.I = 1;
	}

	m_AttachedRigidBody->m_Body->SetMassData(&massdata);
}