#include "Components.hpp"

Camera::Camera(GameObject* gameObject) : Component(gameObject) {
	m_AspectRatio = static_cast<double>(Screen.GetScreenWidth()) / static_cast<double>(Screen.GetScreenHeight());
}

std::unique_ptr<Component> Camera::Clone() const {
	return std::make_unique<Camera>(*this);
}

Vector2D Camera::ViewportToScreenPoint(const Vector2D pos) const {
	Vector2D screenPosition = pos;

	screenPosition.x *= Screen.GetScreenWidth();
	screenPosition.y *= Screen.GetScreenHeight();

	return screenPosition;
}

Vector2D Camera::WorldToViewportPoint(const Vector2D pos) const {
	Vector2D positionRelativeToCamera(pos.x - transform->position.x, pos.y - transform->position.y);

	Vector2D unitsOnScreen(10 * m_AspectRatio * transform->scale.x, 10 * transform->scale.y);

	Vector2D viewportPosition{};

	viewportPosition.x = (unitsOnScreen.x / 2.0) + positionRelativeToCamera.x;
	viewportPosition.y = (unitsOnScreen.y / 2.0) - positionRelativeToCamera.y;

	viewportPosition.x /= unitsOnScreen.x;
	viewportPosition.y /= unitsOnScreen.y;

	return viewportPosition;
}

Vector2D Camera::WorldToScreenPoint(const Vector2D pos) const {
	Vector2D viewportPosition = WorldToViewportPoint(pos);
	Vector2D screenPosition = ViewportToScreenPoint(viewportPosition);

	return screenPosition;
}