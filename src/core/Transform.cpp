#include "Core.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

Transform::Transform(GameObject* gameObject) : gameObject(gameObject), parent(nullptr), tag(&gameObject->tag), name(&gameObject->name) { };

Transform::~Transform() { }

void Transform::Translate(const Vector2D& translation) {
    position.x += translation.x;
    position.y += translation.y;

    for(auto& child : m_Children) {
        child->Translate(translation);
    }
}

void Transform::Rotate(const double angle) {
    rotation += angle;

    for(auto& child : m_Children) {
        child->RotateAround(position, angle);
    }
}

void Transform::RotateAround(const Vector2D& point, const double angle) {
    const double angleRadians = angle * 180 / M_PI;

    const double relX = position.x - point.x, relY = position.y - point.y;
    const double radius = std::sqrt(relX * relX + relY * relY);

    const double currentAngleFromPoint = atan2(relY, relX);

    const double totalAngle = angleRadians + currentAngleFromPoint;

    position.x = (cos(totalAngle) * radius) + point.x;
    position.y = (sin(totalAngle) * radius) + point.y;
    rotation += angle;

    for(auto& child : m_Children) {
        child->RotateAround(point, angle);
    }
}

void Transform::DetachChildren() {
    for(auto& child : m_Children) {
        child->parent = nullptr;
    }

    m_Children.clear();
    childCount = 0;
}

void Transform::DetachFromParent() {
    if(!parent) {
        return;
    }

    auto it = std::find(std::begin(parent->m_Children), std::end(parent->m_Children), this);
    parent->m_Children.erase(it);

    parent = nullptr;
}

Transform* Transform::Find(const std::string_view searchName) const {
    for(const auto& child : m_Children) {
        if(*child->name == searchName) {
            return child;
        }
    }

    return nullptr;
}

Transform* Transform::GetChild(const std::size_t index) const {
    if(index >= childCount) {
        return nullptr;
    }

    return m_Children[index];
}

size_t Transform::GetSiblingIndex() const {
    if(!parent) {
        return 0;
    }

    auto it = std::find(std::begin(parent->m_Children), std::end(parent->m_Children), this);

    return std::distance(std::begin(parent->m_Children), it);
}

void Transform::SetAsFirstSibling() const {
    if(!parent) {
        return;
    }

    std::size_t index = GetSiblingIndex();

    std::swap(parent->m_Children[index], parent->m_Children.front());
}

void Transform::SetAsLastSibling() const {
    if(!parent) {
        return;
    }

    size_t index = GetSiblingIndex();

    std::swap(parent->m_Children[index], parent->m_Children.back());
}

void Transform::SetSiblingIndex(const std::size_t index) const {
    if(!parent) {
        return;
    }

    std::size_t ind = std::clamp(static_cast<std::size_t>(index), static_cast<std::size_t>(0), parent->childCount - 1);
    std::size_t currentIndex = GetSiblingIndex();

    std::swap(parent->m_Children[currentIndex], parent->m_Children[ind]);
}

bool Transform::IsChildOf(Transform* parentTransform) const {
    if(parent == parentTransform) {
        return true;
    }

    return false;
}

bool Transform::IsChildOf(Transform& parentTransform) const {
    return IsChildOf(&parentTransform);
}

bool Transform::IsChildOf(GameObject* parentGo) const {
    return IsChildOf(&parentGo->transform);
}

void Transform::SetParent(Transform* parentTransform) {
    DetachFromParent();

    parent = parentTransform;
    parent->m_Children.push_back(this);
    parent->childCount++;
}

void Transform::SetParent(Transform& parentTransform) {
    SetParent(&parentTransform);
}

void Transform::SetParent(GameObject* parentGo) {
    SetParent(&parentGo->transform);
}