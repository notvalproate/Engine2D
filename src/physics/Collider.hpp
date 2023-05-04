#pragma once
#include "SDL.h"
#include "../math/Vector2d.h"
#include <vector>

class Collider {
public:
	Collider(SDL_Renderer* p_Renderer, const int& p_Width, const int& p_Height, const int& p_OffsetX, const int& p_OffsetY);
	~Collider();

	void SetColliderMap(unsigned short* p_ColliderMap, const int& p_MapWidth, const int& p_MapHeight);
	void LinkObject(Vector2d* p_CurrVelocity, Vector2d* p_CurrPosition, Vector2d* p_LastPosition, bool *p_Jumping);

	void Update(const float& p_DeltaTime);
	void DebugRender(const float& p_DeltaTime);

	struct CollidedTile {
		SDL_Rect m_Tile; 
		double m_TimeHitNear; 
		Vector2d m_ContactNormal; 
	};
private:
	Vector2d *m_CurrPosition, *m_LastPosition, *m_CurrVelocity;
	Vector2d m_ColliderOffset, m_ColliderLastPos;
	bool* m_Jumping;
	 
	SDL_Rect m_ColliderRect;

	SDL_Renderer* m_Renderer;
	SDL_Texture* m_Buffer;

	std::vector<CollidedTile> m_CollidedTiles;

	bool m_CollidesWithMap;
	unsigned short* m_ColliderMap;
	int m_MapWidth, m_MapHeight;

	void ResolveMapCollision(const Vector2d& p_ContactNormal, const SDL_Rect& p_Tile, const float& p_TimeHitNear);
	void CollisionWithMap(const float& p_DeltaTime);
};