#include "Collider.hpp"

StaticCollider2D::StaticCollider2D(const int& p_Width, const int& p_Height, const int& p_X, const int& p_Y) {
	m_ColliderRect = { p_X, p_Y, p_Width, p_Height };
}

void StaticCollider2D::LinkObject(const Vector2d& p_Position) {
	m_ColliderRect.x = (int)p_Position.x;
	m_ColliderRect.y = (int)p_Position.y;
}

void StaticCollider2D::Update(const float& p_DeltaTime) {
	//Updating positiong based on screen movement
}