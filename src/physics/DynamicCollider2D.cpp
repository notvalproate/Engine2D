#include "Collider.hpp"
#include <algorithm>
#include "../math/Rectangle.hpp"

DynamicCollider2D::DynamicCollider2D(const unsigned short p_TileSize, const int& p_Width, const int& p_Height, const int& p_OffsetX, const int& p_OffsetY) : StaticCollider2D(p_Width, p_Height, 0, 0) {
	m_ColliderOffset.x = p_OffsetX;
	m_ColliderOffset.y = p_OffsetY;
	m_TileSize = p_TileSize;

	m_CurrPosition = m_CurrVelocity = m_LastPosition = nullptr;
	m_Jumping = nullptr;
}

void DynamicCollider2D::LinkObject(Vector2d* p_CurrVelocity, Vector2d* p_CurrPosition, Vector2d* p_LastPosition, bool* p_Jumping) {
	m_CurrPosition = p_CurrPosition;
	m_LastPosition = p_LastPosition;
	m_CurrVelocity = p_CurrVelocity;
	m_Jumping = p_Jumping;
}

void DynamicCollider2D::SetColliderMap(unsigned short* p_ColliderMap, const int& p_MapWidth, const int& p_MapHeight) {
	m_ColliderMap = p_ColliderMap;

	if (m_ColliderMap == nullptr) {
		m_CollidesWithMap = false;
		return;
	}

	m_CollidesWithMap = true;
	m_MapWidth = p_MapWidth;
	m_MapHeight = p_MapHeight;
}

void DynamicCollider2D::Update(const float& p_DeltaTime) {
	m_ColliderLastPos = *m_LastPosition + m_ColliderOffset;

	m_ColliderRect.x = (int)(m_CurrPosition->x + m_ColliderOffset.x);
	m_ColliderRect.y = (int)(m_CurrPosition->y + m_ColliderOffset.y); 

	if (m_CollidesWithMap) {
		CollisionWithMap(p_DeltaTime);
	}
}

bool DynamicCollider2D::CollisionWithMap(const float& p_DeltaTime) {
	std::tuple<Vector2d, Vector2d> t_RangeOfCollidableTiles = GetRangeOfTiles();

	GetCollidedTiles(t_RangeOfCollidableTiles, p_DeltaTime);

	if (m_CollidedTiles.empty()) {
		return false;
	}

	SortCollidedTilesByTime();

	ResolveCollisionsInOrder(p_DeltaTime);

	return true;
}


std::tuple<Vector2d, Vector2d> DynamicCollider2D::GetRangeOfTiles() {
	Vector2d t_TopLeftTile, t_BottomRightTile;

	//Get the closest multiple of N to X such that the multiple is < X, eg. N = 4; X = 26.4; return 24;
	auto ClosestMultipleDown = [](const double& p_X, const int& p_N) {
		//alternatively: return (int)p_X - ((int)p_X % p_N);
		return ((int)p_X / p_N) * p_N;
	};

	t_TopLeftTile.x = ClosestMultipleDown(m_ColliderLastPos.x, m_TileSize);
	t_TopLeftTile.y = ClosestMultipleDown(m_ColliderLastPos.y, m_TileSize);

	t_BottomRightTile.x = ClosestMultipleDown(m_ColliderRect.x + m_ColliderRect.w, m_TileSize);
	t_BottomRightTile.y = ClosestMultipleDown(m_ColliderRect.y + m_ColliderRect.h, m_TileSize);

	if (m_LastPosition->x > m_CurrPosition->x) {
		t_TopLeftTile.x = ClosestMultipleDown(m_ColliderRect.x, m_TileSize);
		t_BottomRightTile.x = ClosestMultipleDown(m_ColliderLastPos.x + m_ColliderRect.w, m_TileSize);
	}
	if (m_LastPosition->y > m_CurrPosition->y) {
		t_TopLeftTile.y = ClosestMultipleDown(m_ColliderRect.y, m_TileSize);
		t_BottomRightTile.y = ClosestMultipleDown(m_ColliderLastPos.y + m_ColliderRect.h, m_TileSize);
	}

	t_TopLeftTile.x = std::clamp((int)t_TopLeftTile.x, 0, (m_MapWidth - 1) * m_TileSize);
	t_TopLeftTile.y = std::clamp((int)t_TopLeftTile.y, 0, (m_MapHeight - 1) * m_TileSize);
	t_BottomRightTile.x = std::clamp((int)t_BottomRightTile.x, 0, (m_MapWidth - 1) * m_TileSize);
	t_BottomRightTile.y = std::clamp((int)t_BottomRightTile.y, 0, (m_MapHeight - 1) * m_TileSize);

	return std::make_tuple(t_TopLeftTile, t_BottomRightTile);
}

void DynamicCollider2D::GetCollidedTiles(const std::tuple<Vector2d, Vector2d>& p_RangeOfCollidableTiles, const float& p_DeltaTime) {
	const Vector2d& t_TopLeft = std::get<0>(p_RangeOfCollidableTiles);
	const Vector2d& t_BottomRight = std::get<1>(p_RangeOfCollidableTiles);

	m_CollidedTiles.clear();
	for (int i = (int)t_TopLeft.x; i <= t_BottomRight.x; i += m_TileSize) {
		for (int j = (int)t_TopLeft.y; j <= t_BottomRight.y; j += m_TileSize) {
			CheckCollisionWithTile(SDL_Rect{ i, j, m_TileSize, m_TileSize }, p_DeltaTime);
		}
	}
}

void DynamicCollider2D::CheckCollisionWithTile(const SDL_Rect& p_TileToCheck, const float& p_DeltaTime) {
	Vector2d t_ContactPoint, t_ContactNormal;
	double t_TimeHitNear;

	if (m_ColliderMap[(p_TileToCheck.x / m_TileSize) + (m_MapWidth * p_TileToCheck.y / m_TileSize)] != 0) {
		SDL_FRect t_ColliderFRect = { (float)m_ColliderRect.x, (float)m_ColliderRect.y, (float)m_ColliderRect.w, (float)m_ColliderRect.h };

		if (RectUtil::DynamicRectIntersectRect(t_ColliderFRect, p_TileToCheck, *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
			m_CollidedTiles.push_back(CollidedTile{ p_TileToCheck, t_TimeHitNear, t_ContactNormal });
		}
	}
}

void DynamicCollider2D::SortCollidedTilesByTime() {
	auto CollidedFirst = [](const CollidedTile& p_A, const CollidedTile& p_B) {
		return p_A.m_TimeHitNear < p_B.m_TimeHitNear;
	};

	std::sort(m_CollidedTiles.begin(), m_CollidedTiles.end(), CollidedFirst);
}

void DynamicCollider2D::ResolveCollisionsInOrder(const float& p_DeltaTime) {
	Vector2d t_ContactPoint, t_ContactNormal;
	double t_TimeHitNear;

	ResolveMapCollision(m_CollidedTiles[0].m_ContactNormal, m_CollidedTiles[0].m_Tile, m_CollidedTiles[0].m_TimeHitNear);
	if (*m_Jumping && m_CollidedTiles[0].m_ContactNormal.y == -1) {
		*m_Jumping = false;
	}

	for (unsigned int i = 1; i < m_CollidedTiles.size(); i++) {
		SDL_FRect t_ColliderFRect = { (float)m_ColliderRect.x, (float)m_ColliderRect.y, (float)m_ColliderRect.w, (float)m_ColliderRect.h };

		if (RectUtil::DynamicRectIntersectRect(t_ColliderFRect, m_CollidedTiles[i].m_Tile, *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
			ResolveMapCollision(t_ContactNormal, m_CollidedTiles[i].m_Tile, t_TimeHitNear);
			if (*m_Jumping && t_ContactNormal.y == -1) {
				*m_Jumping = false;
			}
		}
	}
}

void DynamicCollider2D::ResolveMapCollision(const Vector2d& p_ContactNormal, const SDL_Rect& p_Tile, const double& p_TimeHitNear) {
	if (p_ContactNormal.x) {
		//Left
		if (p_ContactNormal.x > 0) {
			m_CurrPosition->x = p_Tile.x + m_TileSize - m_ColliderOffset.x;
		}
		//Right
		else {
			m_CurrPosition->x = p_Tile.x - m_ColliderRect.w - m_ColliderOffset.x;
		}
		m_CurrVelocity->x = 0;
		m_ColliderRect.x = (int)(m_CurrPosition->x + m_ColliderOffset.x);
	}
	if (p_ContactNormal.y) {
		//Top
		if (p_ContactNormal.y > 0) {
			m_CurrPosition->y = p_Tile.y + m_TileSize - m_ColliderOffset.y;
		}
		//Bottom
		else {
			m_CurrPosition->y = p_Tile.y - m_ColliderRect.h - m_ColliderOffset.y;
		}
		m_CurrVelocity->y = 0;
		m_ColliderRect.y = (int)(m_CurrPosition->y + m_ColliderOffset.y);
	}
}