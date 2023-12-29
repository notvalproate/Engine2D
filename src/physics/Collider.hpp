#pragma once
#include "SDL.h"
#include "../math/Vector2d.h"
#include "../Player.hpp"
#include <vector>
#include <memory>

class StaticCollider2D {
public:
	StaticCollider2D(const int& p_Width, const int& p_Height, const int& p_X, const int& p_Y);

	virtual void LinkObject(const Vector2d& p_Position);
	virtual void Update(const float& p_DeltaTime);
protected:
	SDL_Rect m_ColliderRect;

	friend class ColliderDebugRenderer;
};

class DynamicCollider2D : private StaticCollider2D {
public:
	DynamicCollider2D(const unsigned short p_TileSize, const int& p_Width, const int& p_Height, const int& p_OffsetX, const int& p_OffsetY);

	void SetColliderMap(std::shared_ptr<unsigned short[]> p_ColliderMap, const int& p_MapWidth, const int& p_MapHeight);
	void SetPlayer(const std::unique_ptr<Player>& p_Player);
	void Update(const float& p_DeltaTime);

private:
	Vector2d* m_CurrPosition, * m_LastPosition, * m_CurrVelocity;
	Vector2d m_ColliderOffset, m_ColliderLastPos;
	bool* m_Jumping;

	bool m_CollidesWithMap = false;
	std::shared_ptr<unsigned short[]> m_ColliderMap;
	int m_MapWidth = 0, m_MapHeight = 0;
	unsigned short m_TileSize;

	struct CollidedTile {
		SDL_Rect m_Tile;
		double m_TimeHitNear;
		Vector2d m_ContactNormal;
	};
	std::vector<CollidedTile> m_CollidedTiles;

	bool CollisionWithMap(const float& p_DeltaTime);
	std::tuple<Vector2d, Vector2d> GetRangeOfTiles();
	void GetCollidedTiles(const std::tuple<Vector2d, Vector2d>& p_RangeOfCollidableTiles, const float& p_DeltaTime);
	void CheckCollisionWithTile(const SDL_Rect& p_TileToCheck, const float& p_DeltaTime);
	void SortCollidedTilesByTime();
	void ResolveCollisionsInOrder(const float& p_DeltaTime);
	void ResolveMapCollision(const Vector2d& p_ContactNormal, const SDL_Rect& p_Tile, const double& p_TimeHitNear);

	friend class ColliderDebugRenderer;
};

class ColliderDebugRenderer {
public:
	ColliderDebugRenderer(SDL_Renderer* p_Renderer);

	void DebugRender(const DynamicCollider2D& p_Collider, const float& p_DeltaTime);
private:
	SDL_Renderer* m_Renderer;
	SDL_Texture* m_Buffer;
};