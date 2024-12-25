#pragma once

#include "Component.hpp"

class Camera final : public Component {
public:
    ~Camera();

    Vector2D ScreenToViewportPoint(const Vector2D& pos) const;
    Vector2D ScreenToWorldPoint(const Vector2D& pos) const;

    Vector2D ViewportToScreenPoint(const Vector2D& pos) const;
    Vector2D ViewportToWorldPoint(const Vector2D& pos) const;

    Vector2D WorldToScreenPoint(const Vector2D& pos) const;
    Vector2D WorldToViewportPoint(const Vector2D& pos) const;

    double GetPixelsPerUnit() const;

private:
    explicit Camera(GameObject* gameObject);

    std::unique_ptr<Component> Clone() const;

    Vector2D GetUnitsOnScreen() const;

    static constexpr uint8_t defaultUnitsY = 10;

    friend class GameObject;
};