#pragma once

#include "Vector2D.hpp"
#include "components/Components.hpp"

struct RayCastHit {
    bool hit{false};
    Vector2D point;
    Vector2D normal;
    float fraction{0.0f};
    float distance{0.0f};
    Collider* collider{nullptr};

    RayCastHit() {}
};

class PhysicsHandler {
public:
    void SetRenderColliders(const bool set);
    RayCastHit RayCast(const Vector2D& origin, const Vector2D& direction, float distance) const;

    void SetGravity(const Vector2D& gravity) const;
    Vector2D GetGravity() const;

    double bouncinessThreshold;
private:
    PhysicsHandler();

    void RenderColliders() const;

    class ContactListener : public b2ContactListener {
        void BeginContact(b2Contact* contact) override;
        void EndContact(b2Contact* contact) override;

    private:
        Collision GetCollision(Collider* collider, Collider* otherCollider) const;
    };

    class RayCastCallback : public b2RayCastCallback {
    public:
        float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
        inline bool DidHit() const { return m_Result.hit; }
        inline RayCastHit& GetResult() { return m_Result; }
        inline b2Fixture* GetFixture() { return m_FixtureHit; }

    private:
        RayCastHit m_Result;
        b2Fixture* m_FixtureHit;
    };

    bool m_RenderSceneColliders;
    ContactListener m_ContactListener;

    friend class Object;
    friend class Scene;
    friend class Engine2D;
    friend class RenderingHandler;
    friend class BoxCollider;
    friend class RayCastCallback;
};