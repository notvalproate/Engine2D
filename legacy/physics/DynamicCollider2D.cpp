#include "Collider.hpp"
#include <algorithm>
#include "../math/Rectangle.hpp"

namespace notval {

	DynamicCollider2D::DynamicCollider2D(const int width, const int height, const int offsetX, const int offsetY)
		: StaticCollider2D(width, height, 0, 0), m_CollisionLayer(nullptr)
	{
		m_ColliderOffset.x = offsetX;

		m_ColliderOffset.y = offsetY;

		m_CurrPosition = m_CurrVelocity = m_LastPosition = nullptr;
		m_Jumping = nullptr;
		m_CollidesWithMap = false;
		m_TileSize = 0;
	}

	void DynamicCollider2D::SetCollisionLayer(const std::unique_ptr<Tilemap>& collider) {
		m_CollisionLayer = collider->GetCollisionLayer();

		if (m_CollisionLayer->name == "") {
			return;
		}

		m_TileSize = collider->GetTileSize();
		m_CollidesWithMap = true;
	}

	void DynamicCollider2D::SetPlayer(const std::unique_ptr<Player>& player) {
		m_CurrPosition = &player->m_CurrPosition;
		m_LastPosition = &player->m_LastPosition;
		m_CurrVelocity = &player->m_CurrVelocity;
		m_Jumping = &player->m_Jumping;
	}

	void DynamicCollider2D::Update(const float deltaTime) {
		m_ColliderLastPos = *m_LastPosition + m_ColliderOffset;

		m_ColliderRect.x = (int)(m_CurrPosition->x + m_ColliderOffset.x);
		m_ColliderRect.y = (int)(m_CurrPosition->y + m_ColliderOffset.y);

		if (m_CollidesWithMap) {
			CollisionWithMap(deltaTime);
		}
	}

	bool DynamicCollider2D::CollisionWithMap(const float deltaTime) {
		std::tuple<Vector2d, Vector2d> rangeOfCollidableTiles = GetRangeOfTiles();

		GetCollidedTiles(rangeOfCollidableTiles, deltaTime);

		if (m_CollidedTiles.empty()) {
			return false;
		}

		SortCollidedTilesByTime();

		ResolveCollisionsInOrder(deltaTime);

		return true;
	}


	std::tuple<Vector2d, Vector2d> DynamicCollider2D::GetRangeOfTiles() {
		Vector2d topLeftTile, bottomRightTile;

		//Get the closest multiple of N to X such that the multiple is < X, eg. N = 4; X = 26.4; return 24;
		auto ClosestMultipleDown = [](const double X, const int N) {
			//alternatively: return (int)X - ((int)X % N);
			return ((int)X / N) * N;
		};

		topLeftTile.x = ClosestMultipleDown(m_ColliderLastPos.x, m_TileSize);
		topLeftTile.y = ClosestMultipleDown(m_ColliderLastPos.y, m_TileSize);

		bottomRightTile.x = ClosestMultipleDown(m_ColliderRect.x + m_ColliderRect.w, m_TileSize);
		bottomRightTile.y = ClosestMultipleDown(m_ColliderRect.y + m_ColliderRect.h, m_TileSize);

		if (m_LastPosition->x > m_CurrPosition->x) {
			topLeftTile.x = ClosestMultipleDown(m_ColliderRect.x, m_TileSize);
			bottomRightTile.x = ClosestMultipleDown(m_ColliderLastPos.x + m_ColliderRect.w, m_TileSize);
		}
		if (m_LastPosition->y > m_CurrPosition->y) {
			topLeftTile.y = ClosestMultipleDown(m_ColliderRect.y, m_TileSize);
			bottomRightTile.y = ClosestMultipleDown(m_ColliderLastPos.y + m_ColliderRect.h, m_TileSize);
		}

		topLeftTile.x = std::clamp((int)topLeftTile.x, 0, (m_CollisionLayer->width - 1) * m_TileSize);
		topLeftTile.y = std::clamp((int)topLeftTile.y, 0, (m_CollisionLayer->height - 1) * m_TileSize);
		bottomRightTile.x = std::clamp((int)bottomRightTile.x, 0, (m_CollisionLayer->width - 1) * m_TileSize);
		bottomRightTile.y = std::clamp((int)bottomRightTile.y, 0, (m_CollisionLayer->height - 1) * m_TileSize);

		return std::make_tuple(topLeftTile, bottomRightTile);
	}

	void DynamicCollider2D::GetCollidedTiles(const std::tuple<Vector2d, Vector2d>& rangeOfCollidableTiles, const float deltaTime) {
		const Vector2d& topLeft = std::get<0>(rangeOfCollidableTiles);
		const Vector2d& bottomRight = std::get<1>(rangeOfCollidableTiles);

		m_CollidedTiles.clear();
		for (int i = (int)topLeft.x; i <= bottomRight.x; i += m_TileSize) {
			for (int j = (int)topLeft.y; j <= bottomRight.y; j += m_TileSize) {
				CheckCollisionWithTile(SDL_Rect{ i, j, m_TileSize, m_TileSize }, deltaTime);
			}
		}
	}

	void DynamicCollider2D::CheckCollisionWithTile(const SDL_Rect& tileToCheck, const float deltaTime) {
		Vector2d contactPoint, contactNormal;
		double timeHitNear;

		if (m_CollisionLayer->data[(tileToCheck.x / m_TileSize) + (m_CollisionLayer->width * tileToCheck.y / m_TileSize)] != 0) {
			SDL_FRect colliderFRect = { (float)m_ColliderRect.x, (float)m_ColliderRect.y, (float)m_ColliderRect.w, (float)m_ColliderRect.h };

			if (RectUtil::DynamicRectIntersectRect(colliderFRect, tileToCheck, *m_CurrVelocity, contactPoint, contactNormal, timeHitNear, deltaTime)) {
				m_CollidedTiles.push_back(CollidedTile{ tileToCheck, timeHitNear, contactNormal });
			}
		}
	}

	void DynamicCollider2D::SortCollidedTilesByTime() {
		auto CollidedFirst = [](const CollidedTile& A, const CollidedTile& B) {
			return A.m_TimeHitNear < B.m_TimeHitNear;
		};

		std::sort(m_CollidedTiles.begin(), m_CollidedTiles.end(), CollidedFirst);
	}

	void DynamicCollider2D::ResolveCollisionsInOrder(const float deltaTime) {
		Vector2d contactPoint, contactNormal;
		double timeHitNear;

		ResolveMapCollision(m_CollidedTiles[0].m_ContactNormal, m_CollidedTiles[0].m_Tile, m_CollidedTiles[0].m_TimeHitNear);
		if (*m_Jumping && m_CollidedTiles[0].m_ContactNormal.y == -1) {
			*m_Jumping = false;
		}

		for (unsigned int i = 1; i < m_CollidedTiles.size(); i++) {
			SDL_FRect colliderFRect = { (float)m_ColliderRect.x, (float)m_ColliderRect.y, (float)m_ColliderRect.w, (float)m_ColliderRect.h };

			if (RectUtil::DynamicRectIntersectRect(colliderFRect, m_CollidedTiles[i].m_Tile, *m_CurrVelocity, contactPoint, contactNormal, timeHitNear, deltaTime)) {
				ResolveMapCollision(contactNormal, m_CollidedTiles[i].m_Tile, timeHitNear);
				if (*m_Jumping && contactNormal.y == -1) {
					*m_Jumping = false;
				}
			}
		}
	}

	void DynamicCollider2D::ResolveMapCollision(const Vector2d& contactNormal, const SDL_Rect& tile, const double timeHitNear) {
		if (contactNormal.x) {
			//Left
			if (contactNormal.x > 0) {
				m_CurrPosition->x = tile.x + m_TileSize - m_ColliderOffset.x;
			}
			//Right
			else {
				m_CurrPosition->x = tile.x - m_ColliderRect.w - m_ColliderOffset.x;
			}
			m_CurrVelocity->x = 0;
			m_ColliderRect.x = (int)(m_CurrPosition->x + m_ColliderOffset.x);
		}
		if (contactNormal.y) {
			//Top
			if (contactNormal.y > 0) {
				m_CurrPosition->y = tile.y + m_TileSize - m_ColliderOffset.y;
			}
			//Bottom
			else {
				m_CurrPosition->y = tile.y - m_ColliderRect.h - m_ColliderOffset.y;
			}
			m_CurrVelocity->y = 0;
			m_ColliderRect.y = (int)(m_CurrPosition->y + m_ColliderOffset.y);
		}
	}

}