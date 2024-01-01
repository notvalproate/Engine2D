#include "Collider.hpp"

StaticCollider2D::StaticCollider2D(const int width, const int height, const int X, const int Y) {
	m_ColliderRect = { X, Y, width, height };
}

void StaticCollider2D::LinkObject(const Vector2d& position) {
	m_ColliderRect.x = (int)position.x;
	m_ColliderRect.y = (int)position.y;
}

void StaticCollider2D::Update(const float deltaTime) {
	//Updating positiong based on screen movement
}