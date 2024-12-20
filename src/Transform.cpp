#include "Core.hpp"
#include <math.h>

Transform::Transform(GameObject* gameObject) : gameObject(gameObject), tag(&gameObject->tag), name(&gameObject->name), m_Parent(nullptr) { };

void Transform::Translate(const Vector2D& translation) {
    position.x += translation.x;
    position.y += translation.y;

    for(auto& child : m_Children) {
        child->Translate(translation);
    }
}

void Transform::AddToRotation(const double angle) {
    rotation += angle;

    if (rotation > 360.0) {
        rotation -= 360;
    }
    else if (rotation < 0.0) {
        rotation += 360;
    }
}

void Transform::Rotate(const double angle) {
    AddToRotation(angle);

    for(auto& child : m_Children) {
        child->RotateAround(position, angle);
    }
}

void Transform::RotateAround(const Vector2D& point, const double angle) {
    position.RotateAround(point, angle);
    AddToRotation(angle);

    for(auto& child : m_Children) {
        child->RotateAround(point, angle);
    }
}

void Transform::Scale(const Vector2D& scaleSet) {
    scale.x *= scaleSet.x;
    scale.y *= scaleSet.y;

    for (auto& child : m_Children) {
        child->Scale(scale);
    }
}

void Transform::SetScale(const Vector2D& scaleSet) {
    scale = scaleSet;

    for (auto& child : m_Children) {
        child->SetScale(scale);
    }
}

void Transform::DetachChildren() {
    for(auto& child : m_Children) {
        child->m_Parent = nullptr;
    }

    m_Children.clear();
    childCount = 0;
}

void Transform::DetachFromParent() {
    if(!m_Parent) {
        return;
    }

    auto it = std::find(std::begin(m_Parent->m_Children), std::end(m_Parent->m_Children), this);
    m_Parent->m_Children.erase(it);

    m_Parent = nullptr;
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
    if(!m_Parent) {
        return 0;
    }

    auto it = std::find(std::begin(m_Parent->m_Children), std::end(m_Parent->m_Children), this);

    return std::distance(std::begin(m_Parent->m_Children), it);
}

void Transform::SetAsFirstSibling() const {
    if(!m_Parent) {
        return;
    }

    std::size_t index = GetSiblingIndex();

    std::swap(m_Parent->m_Children[index], m_Parent->m_Children.front());
}

void Transform::SetAsLastSibling() const {
    if(!m_Parent) {
        return;
    }

    size_t index = GetSiblingIndex();

    std::swap(m_Parent->m_Children[index], m_Parent->m_Children.back());
}

void Transform::SetSiblingIndex(const std::size_t index) const {
    if(!m_Parent) {
        return;
    }

    std::size_t ind = std::clamp(static_cast<std::size_t>(index), static_cast<std::size_t>(0), m_Parent->childCount - 1);
    std::size_t currentIndex = GetSiblingIndex();

    std::swap(m_Parent->m_Children[currentIndex], m_Parent->m_Children[ind]);
}

bool Transform::IsChildOf(Transform* parentTransform) const {
    return m_Parent == parentTransform;
}

bool Transform::IsChildOf(Transform& parentTransform) const {
    return IsChildOf(&parentTransform);
}

bool Transform::IsChildOf(GameObject* parentGo) const {
    return IsChildOf(&parentGo->transform);
}

void Transform::SetParent(Transform* parentTransform, bool instantiateInWorldSpace) {
    DetachFromParent();

    m_Parent = parentTransform;
    m_Parent->m_Children.push_back(this);
    m_Parent->childCount++;

    if (!instantiateInWorldSpace) {
        position += m_Parent->position;
        rotation += m_Parent->rotation;
        scale.x *= m_Parent->scale.x;
        scale.y *= m_Parent->scale.y;
    }
}

void Transform::SetParent(Transform& parentTransform, bool instantiateInWorldSpace) {
    SetParent(&parentTransform,  instantiateInWorldSpace);
}

void Transform::SetParent(GameObject* parentGo, bool instantiateInWorldSpace) {
    SetParent(&parentGo->transform, instantiateInWorldSpace);
}