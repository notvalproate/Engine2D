#include "Components.hpp"

Camera::Camera(GameObject* gameObject) : Component(gameObject) {
	m_AspectRatio = static_cast<double>(Screen.GetScreenWidth()) / static_cast<double>(Screen.GetScreenHeight());
}

std::unique_ptr<Component> Camera::Clone() const {
	return std::make_unique<Camera>(*this);
}

Vector2D Camera::WorldToViewportPoint(const Vector2D pos) const {
	Vector2D viewportPosition(pos.x - transform->position.x, pos.y - transform->position.y);

	Vector2D defaultUnits(10 * m_AspectRatio * transform->scale.x, 10 * transform->scale.y);

	viewportPosition.x = (defaultUnits.x / 2.0) + viewportPosition.x;
	viewportPosition.y = (defaultUnits.y / 2.0) - viewportPosition.y;

	viewportPosition.x /= defaultUnits.x;
	viewportPosition.y /= defaultUnits.y;

	return viewportPosition;
}

Vector2D Camera::WorldToScreenPoint(const Vector2D pos) const {
	Vector2D screenPosition = WorldToViewportPoint(pos);

	screenPosition.x *= Screen.GetScreenWidth();
	screenPosition.y *= Screen.GetScreenHeight();

	return screenPosition;
}