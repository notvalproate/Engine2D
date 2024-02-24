#include "Components.hpp"

Camera::Camera(GameObject* gameObject) : Component(gameObject) { }

std::unique_ptr<Component> Camera::Clone() const {
	return std::make_unique<Camera>(*this);
}

Vector2D Camera::GetUnitsOnScreen() const { 
	return Vector2D(
		defaultUnitsY * Screen.GetAspectRatio() * transform->scale.x,
		defaultUnitsY * transform->scale.y
	);
}

double Camera::GetPixelsPerUnit() const {
	return Screen.GetScreenHeight() / (defaultUnitsY * transform->scale.y);
}

Vector2D Camera::ScreenToViewportPoint(const Vector2D& pos) const {
	return Vector2D(
		pos.x / Screen.GetScreenWidth(),
		pos.y / Screen.GetScreenHeight()
	);
}

Vector2D Camera::ScreenToWorldPoint(const Vector2D& pos) const {
	return ViewportToWorldPoint( ScreenToViewportPoint(pos) );
}

Vector2D Camera::ViewportToScreenPoint(const Vector2D& pos) const {
	return Vector2D(
		pos.x * Screen.GetScreenWidth(),
		pos.y * Screen.GetScreenHeight()
	);
}

Vector2D Camera::ViewportToWorldPoint(const Vector2D& pos) const {
	Vector2D unitsOnScreen = GetUnitsOnScreen();
	
	return Vector2D(
		(pos.x - 0.5) * unitsOnScreen.x + transform->position.x,
		(pos.y - 0.5) * unitsOnScreen.y + transform->position.y * -1
	);
}

Vector2D Camera::WorldToViewportPoint(const Vector2D& pos) const {
	Vector2D unitsOnScreen = GetUnitsOnScreen();

	return Vector2D(
		0.5 + ((pos.x - transform->position.x) / unitsOnScreen.x), 
		0.5 - ((pos.y - transform->position.y) / unitsOnScreen.y)
	);
}

Vector2D Camera::WorldToScreenPoint(const Vector2D& pos) const {
	return ViewportToScreenPoint( WorldToViewportPoint(pos) );
}