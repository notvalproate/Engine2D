#pragma once
#include "../Core.hpp"

class SpriteRenderer final : public Component {
public:
	~SpriteRenderer();

	void SetSprite(const char* spritePath);
	void SetSortingLayer(const std::string_view layerName);
	void SetPixelsPerUnit(const uint16_t pixelsPerUnit);

	inline Vector2D GetPixelDimensions() const { return m_Dimensions; }
	inline Vector2D GetWorldDimensions() const { return m_Dimensions / m_PixelsPerUnit; }

private:
	SpriteRenderer(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	void Render() const override;

	SDL_Texture* m_Sprite;
	Vector2D m_Dimensions;
	uint16_t m_PixelsPerUnit;
	std::string m_SortingLayer;

	friend class GameObject;
};

struct PhysicsMaterial {
	double bounciness = 0.0;
	double friction = 0.3f;

	explicit inline PhysicsMaterial() = default;
	explicit inline PhysicsMaterial(const double bounciness, const double friction) : bounciness(bounciness), friction(friction) { }
};

enum class RigidBodyType {
	Static = b2_staticBody,
	Kinematic = b2_kinematicBody,
	Dynamic = b2_dynamicBody
};

// TOTAL FORCE AND TOTAL TORQUE ARE ALWAYS ZERO IN THE UPDATE LOOPS OF BEHAVIOURS
class RigidBody final : public Component {
public:
	~RigidBody();

	void AddForce(const Vector2D& force);
	void AddForceAtPosition(const Vector2D& force, const Vector2D& position);
	void AddRelativeForce(Vector2D force);
	void AddTorque(const double force);

	void SetMass(const float mass);
	void SetAutoMass(const bool use);
	void SetMaterial(const PhysicsMaterial& material);
	void SetVelocity(const Vector2D& vel);
	void SetAngularVelocity(const float vel);
	void SetPosition(const Vector2D& pos);
	void SetRotation(const double rotation);
	void SetBodyType(const RigidBodyType type);
	void SetGravityScale(const double scale);
	void FreezeRotation(const bool set);
	void WakeUp();
	void Sleep();

	float GetMass() const;
	PhysicsMaterial GetMaterial() const;
	Vector2D GetVelocity() const;
	float GetAngularVelocity() const;
	RigidBodyType GetBodyType() const;
	Vector2D GetCentreOfMass() const;
	std::vector<Collider*> GetAttachedColliders() const;
	bool IsAwake() const;
	bool IsSleeping() const;

	double drag;
	double angularDrag;
	Vector2D totalForce;
	double totalTorque;

private:
	RigidBody(GameObject* gameObj);

	void Update() override;
	void AttachCollider(Collider* collider);

	void AddDrag();
	void AddAngularDrag();
	void ApplyTotalForces();

	b2Body* m_Body;
	double m_Mass;
	bool m_AutoMassEnabled;
	PhysicsMaterial m_Material;
	std::vector<Collider*> m_AttachedColliders;
	std::optional<b2Fixture*> m_SensorFixture;

	friend class GameObject;
	friend class Collider;
	friend class PolygonCollider;
	friend class Scene;
};

struct Bounds {
public:
	Vector2D center;
	Vector2D extents;
	Vector2D max;
	Vector2D min;
	Vector2D size;
};

// All colliders must inherit from this class.
class Collider : public Behaviour {
public:
	virtual ~Collider();

	virtual void SetDensity(const double density);
	void SetMaterial(const PhysicsMaterial& material);
	void SetFriction(const double friction);
	void SetBounciness(const double bounciness);

	Vector2D GetCenter() const { return transform->position + m_Offset; }
	Vector2D GetOffset() const { return m_Offset; }
	Bounds GetBounds() const { return m_BoundingBox; }
	double GetDensity() const;
	std::optional<PhysicsMaterial> GetMaterial() const;
	double GetFriction() const;
	double GetBounciness() const;

	RigidBody* attachedRigidBody;
protected:
	Collider(GameObject* gameObj);

	void Awake() override final;
	void Update() override final;
	void UpdateBounds();
	void UpdateStaticPosition();
	void ResetShape();
	void ResetDensity();
	virtual void RemoveFixtureFromMap() const;
	virtual void AddFixtureToMap();

	virtual void AttachRigidBody(RigidBody* rigidBody);
	virtual void DeatachRigidBody();

	void CreateColliderOnRigidBody(const b2Shape* colShape);
	void CreateStaticCollider(const b2Shape* colShape);
	void DestroyStaticCollider();
	b2BodyDef GetStaticBodyDef() const;
	b2FixtureDef GetFixtureDef(const b2Shape* colShape) const;

	virtual b2Shape* GetShape(bool useOffset = false) const = 0;

	b2Fixture* m_Fixture;
	Vector2D m_Offset;
	double m_Rotation;

	Bounds m_BoundingBox;
	std::optional<PhysicsMaterial> m_Material;

	double m_Density;

	Vector2D m_CurrentPosition;
	std::optional<b2Body*> m_StaticBody;

	friend class GameObject;
	friend class RigidBody;
};

// ADD ABILITY TO DISABLE BOXCOLLIDER
class BoxCollider final : public Collider {
public:
	void SetTransform(const Vector2D& dimensions, const Vector2D& offset, const double rotation);
	Vector2D GetSize() const { return m_Dimensions; }

private:
	BoxCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	b2Shape* GetShape(bool useOffset = false) const override;

	Vector2D m_Dimensions;

	friend class GameObject;
	friend class RigidBody;
};

class PolygonCollider final : public Collider {
public:
	~PolygonCollider() override;

	void SetPoints(const std::vector<Vector2D>& points);
	void SetOffset(const Vector2D& offset);
	std::vector<Vector2D> GetPoints() const { return std::vector<Vector2D>(); }

	void SetDensity(const double density) override;
private:
	PolygonCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	b2Shape* GetShape(bool useOffset = false) const override;
	bool ContainsConcavity(const std::vector<Vector2D>& points) const;
	void ReducePointsToPolygons();

	void AttachRigidBody(RigidBody* rigidBody) override;
	void DeatachRigidBody() override;
	void RemoveFixtureFromMap() const override;
	void AddFixtureToMap() override;

	std::vector<Vector2D> m_Points;
	std::vector<std::vector<b2Vec2>> m_ReducedPolygons;
	std::vector<b2Fixture*> m_FixtureVector;

	friend class GameObject;
	friend class RigidBody;
};

class CircleCollider final : public Collider {
public:
	void SetTransform(const double radius, const Vector2D& offset, const double rotation);
	double GetRadius() const { return m_Radius; }

private:
	CircleCollider(GameObject* gameObj);
	std::unique_ptr<Component> Clone() const;

	b2Shape* GetShape(bool useOffset = false) const override;

	double m_Radius;

	friend class GameObject;
	friend class RigidBody;
};