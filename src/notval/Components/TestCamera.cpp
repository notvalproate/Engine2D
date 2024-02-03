#include "Components.hpp"

Camera::Camera(GameObject* gameObject) : Component(gameObject) {
	m_AspectRatio = static_cast<double>(Screen.GetScreenWidth()) / static_cast<double>(Screen.GetScreenHeight());
}

std::unique_ptr<Component> Camera::Clone() const {
	return std::make_unique<Camera>(*this);
}

Vector2D Camera::GetUnitsOnScreen() const { 
	return Vector2D(
		defaultUnitsY * m_AspectRatio * transform->scale.x, 
		defaultUnitsY * transform->scale.y
	);
}

Vector2D Camera::ScreenToViewportPoint(const Vector2D pos) const {
	Vector2D viewportPosition = pos;

	viewportPosition.x /= Screen.GetScreenWidth();
	viewportPosition.y /= Screen.GetScreenHeight();

	return viewportPosition;
}

Vector2D Camera::ScreenToWorldPoint(const Vector2D pos) const {
	Vector2D viewportPosition = ScreenToViewportPoint(pos); 
	Vector2D worldPosition = ViewportToWorldPoint(viewportPosition);

	return worldPosition;
}

Vector2D Camera::ViewportToScreenPoint(const Vector2D pos) const {
	Vector2D screenPosition = pos;

	screenPosition.x *= Screen.GetScreenWidth();
	screenPosition.y *= Screen.GetScreenHeight();

	return screenPosition;
}

Vector2D Camera::ViewportToWorldPoint(const Vector2D pos) const {
	Vector2D unitsOnScreen = GetUnitsOnScreen();
	Vector2D positionRelativeToCamera{};

	positionRelativeToCamera.x = (pos.x - 0.5) * unitsOnScreen.x;
	positionRelativeToCamera.y = (pos.y - 0.5) * unitsOnScreen.y * -1;

	Vector2D worldPosition(positionRelativeToCamera.x + transform->position.x, positionRelativeToCamera.y + transform->position.y);
	
	return worldPosition;
}

Vector2D Camera::WorldToViewportPoint(const Vector2D pos) const {
	Vector2D positionRelativeToCamera(pos.x - transform->position.x, pos.y - transform->position.y);
	Vector2D unitsOnScreen = GetUnitsOnScreen();

	Vector2D viewportPosition{};

	viewportPosition.x = 0.5 + (positionRelativeToCamera.x / unitsOnScreen.x);
	viewportPosition.y = 0.5 - (positionRelativeToCamera.y / unitsOnScreen.y);

	return viewportPosition;
}

Vector2D Camera::WorldToScreenPoint(const Vector2D pos) const {
	Vector2D viewportPosition = WorldToViewportPoint(pos);
	Vector2D screenPosition = ViewportToScreenPoint(viewportPosition);

	return screenPosition;
}