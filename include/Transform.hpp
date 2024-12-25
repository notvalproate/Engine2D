#pragma once

#include <vector>

#include "Vector2D.hpp"

class GameObject;

class Transform final {
public:
    Vector2D position{};
    double rotation{};
    Vector2D scale{Vector2D::one};

    GameObject* gameObject;

    std::size_t childCount{};

    std::string* tag;
    std::string* name;

    void Translate(const Vector2D& translation);
    void Rotate(const double angle);
    void RotateAround(const Vector2D& point, const double angle);
    void Scale(const Vector2D& scaleSet);
    void SetScale(const Vector2D& scaleSet);

    void DetachChildren();
    void DetachFromParent();
    Transform* Find(const std::string_view searchName) const;
    Transform* GetChild(const std::size_t index) const;
    size_t GetSiblingIndex() const;
    void SetAsFirstSibling() const;
    void SetAsLastSibling() const;
    void SetSiblingIndex(const std::size_t index) const;

    bool IsChildOf(Transform* parentTransform) const;
    bool IsChildOf(Transform& parentTransform) const;
    bool IsChildOf(GameObject* parentGo) const;

    void SetParent(Transform* parentTransform, bool instantiateInWorldSpace = false);
    void SetParent(Transform& parentTransform, bool instantiateInWorldSpace = false);
    void SetParent(GameObject* parentGo, bool instantiateInWorldSpace = false);

    inline Transform* GetParent() const { return m_Parent; }
    inline std::vector<Transform*> GetChildren() const { return m_Children; }

private:
    explicit Transform(GameObject* gameObject);

    void AddToRotation(const double angle);

    std::vector<Transform*> m_Children{};
    Transform* m_Parent;

    friend class Object;
    friend class GameObject;
};