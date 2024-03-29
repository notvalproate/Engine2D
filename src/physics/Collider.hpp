#pragma once
#include "SDL.h"
#include "../math/Vector2d.h"
#include "../Player.hpp"
#include "../Camera.hpp"
#include "../tilemap/Tilemap.hpp"
#include <vector>
#include <memory>

namespace notval {

	class StaticCollider2D {
	public:
		StaticCollider2D(const int width, const int height, const int  X, const int Y);

		virtual void LinkObject(const Vector2d& position);
		virtual void Update(const float deltaTime);
	protected:
		SDL_Rect m_ColliderRect;

		friend class ColliderDebugRenderer;
	};

	class DynamicCollider2D : private StaticCollider2D {
	public:
		DynamicCollider2D(const int width, const int height, const int offsetX, const int offsetY);

		void SetCollisionLayer(const std::unique_ptr<Tilemap>& collider);
		void SetPlayer(const std::unique_ptr<Player>& player);
		void Update(const float deltaTime);
	private:
		const Tilemap::Layer* m_CollisionLayer;

		Vector2d* m_CurrPosition, * m_LastPosition, * m_CurrVelocity;
		Vector2d m_ColliderOffset, m_ColliderLastPos;
		bool* m_Jumping;

		bool m_CollidesWithMap = false;
		unsigned short m_TileSize;

		struct CollidedTile {
			SDL_Rect m_Tile;
			double m_TimeHitNear;
			Vector2d m_ContactNormal;
		};
		std::vector<CollidedTile> m_CollidedTiles;

		bool CollisionWithMap(const float deltaTime);
		std::tuple<Vector2d, Vector2d> GetRangeOfTiles();
		void GetCollidedTiles(const std::tuple<Vector2d, Vector2d>& rangeOfCollidableTiles, const float deltaTime);
		void CheckCollisionWithTile(const SDL_Rect& tileToCheck, const float deltaTime);
		void SortCollidedTilesByTime();
		void ResolveCollisionsInOrder(const float deltaTime);
		void ResolveMapCollision(const Vector2d& contactNormal, const SDL_Rect& tile, const double timeHitNear);

		friend class ColliderDebugRenderer;
	};

	class ColliderDebugRenderer {
	public:
		ColliderDebugRenderer(SDL_Renderer* renderer);

		void SetCollisionLayer(const Tilemap::Layer* collider);
		void DebugRender(const DynamicCollider2D& collider, const float deltaTime, const std::unique_ptr<Camera>& camera);
	private:
		const Tilemap::Layer* m_CollisionLayer;

		SDL_Rect m_BufferRect;
		SDL_Renderer* m_Renderer;
		SDL_Texture* m_BackgroundBuffer;
	};

}