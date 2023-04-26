#pragma once
#include "SDL.h"
#include "../math/Vector2d.h"

class Collider {
public:
	Collider(const SDL_Rect& p_ColliderRect, const int& p_OffsetX, const int& p_OffsetY, SDL_Renderer* p_Renderer);
	~Collider();

	void SetColliderMap(unsigned short* p_ColliderMap, const int& p_MapWidth, const int& p_MapHeight);
	void LinkObject(Vector2d* p_CurrVelocity, Vector2d* p_CurrPosition, Vector2d* p_LastPosition);

	void Update(const float& p_DeltaTime);
	void DebugRender(const float& p_DeltaTime);

	void CollisionWith(const Collider& p_OtherCollider);
private:
	Vector2d *m_CurrPosition, *m_LastPosition, *m_CurrVelocity;
	Vector2d m_Offset;

	SDL_Rect m_ColliderRect;

	SDL_Renderer* m_Renderer;
	SDL_Texture* m_Buffer;

	bool m_CollidesWithMap;
	unsigned short* m_ColliderMap;
	int m_MapWidth, m_MapHeight;
	int r, g, b;

	void ResolveCollision(const Collider& p_OtherCollider);
	void ResolveMapCollision(const Vector2d& p_ContactNormal, const SDL_Rect& p_Tile);

	void CollisionWithMap(const float& p_DeltaTime);
};