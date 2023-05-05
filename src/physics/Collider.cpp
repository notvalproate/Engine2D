#include "Collider.hpp"
#include "../math/Rectangle.hpp"
#include <iostream>
#include <algorithm>

Collider::Collider(SDL_Renderer* p_Renderer, const int& p_Width, const int& p_Height, const int& p_OffsetX, const int& p_OffsetY) {
	m_ColliderRect = { 0, 0, p_Width, p_Height };
	m_ColliderOffset.x = p_OffsetX;
	m_ColliderOffset.y = p_OffsetY;

	m_CollidesWithMap = false;
	m_MapWidth = 0;
	m_MapHeight = 0;

	m_Renderer = p_Renderer;
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
}

Collider::~Collider() {
	delete m_Buffer;
}

void Collider::SetColliderMap(unsigned short* p_ColliderMap, const int& p_MapWidth, const int& p_MapHeight) {
	m_ColliderMap = p_ColliderMap;

	if (m_ColliderMap == nullptr) {
		m_CollidesWithMap = false;
		return;
	}
	else {
		m_CollidesWithMap = true; 
		m_MapWidth = p_MapWidth;
		m_MapHeight = p_MapHeight;
	}
}

void Collider::LinkObject(Vector2d* p_CurrVelocity, Vector2d* p_CurrPosition, Vector2d* p_LastPosition, bool* p_Jumping) {
	m_CurrPosition = p_CurrPosition;
	m_LastPosition = p_LastPosition;
	m_CurrVelocity = p_CurrVelocity;
	m_Jumping = p_Jumping;
}

void Collider::Update(const float& p_DeltaTime) {
	m_ColliderLastPos = *m_LastPosition + m_ColliderOffset;

	m_ColliderRect.x = m_CurrPosition->x + m_ColliderOffset.x;
	m_ColliderRect.y = m_CurrPosition->y + m_ColliderOffset.y; 

	if (m_CollidesWithMap) {
		CollisionWithMap(p_DeltaTime);
	}
}

void Collider::DebugRender(const float& p_DeltaTime) {
	//Set target to render debug onto buffer
	SDL_SetRenderTarget(m_Renderer, m_Buffer); 
	SDL_RenderClear(m_Renderer);

	//Debug to render all the map colliders
	bool t_HighlightColliders = true;
	SDL_Rect t_Tile;
	if (t_HighlightColliders && m_CollidesWithMap) {
		int t_Size = m_MapHeight * m_MapWidth;
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 255, 100);
		for (int i = 0; i < t_Size; i++) {
			if (m_ColliderMap[i] == 1) {
				t_Tile = { 8 * (i % m_MapWidth), 8 * (i / m_MapWidth), 8, 8 };
				SDL_RenderFillRect(m_Renderer, &t_Tile);
			}
		}
	}

	//Debug to render hitbox
	SDL_SetRenderDrawColor(m_Renderer, 255, 165, 0, 100);
	SDL_RenderFillRect(m_Renderer, &m_ColliderRect);

	//Debug to render velocity direction 
	int centerx = m_ColliderRect.x + m_ColliderRect.w / 2;
	int centery = m_ColliderRect.y + m_ColliderRect.h / 2;
	SDL_SetRenderDrawColor(m_Renderer, 00, 255, 0, 255);
	SDL_RenderDrawLine(m_Renderer, centerx, centery, centerx + 15 * (m_CurrVelocity->x / m_CurrVelocity->GetMagnitude()), centery + 15 * (m_CurrVelocity->y / m_CurrVelocity->GetMagnitude()));
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);

	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0); 
	SDL_SetRenderTarget(m_Renderer, NULL);
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
}

std::tuple<Vector2d, Vector2d> Collider::GetRangeOfTiles() {
	Vector2d t_TopLeftTile, t_BottomRightTile;

	//Get the closest multiple of N to X such that the multiple is < X, eg. N = 4; X = 26.4; return 24;
	auto ClosestMultipleDown = [](const float& p_X, const int& p_N) {
		return (int)p_X - ((int)p_X % p_N);
	};

	t_TopLeftTile.x = ClosestMultipleDown(m_ColliderLastPos.x, 8);
	t_TopLeftTile.y = ClosestMultipleDown(m_ColliderLastPos.y, 8);

	t_BottomRightTile.x = ClosestMultipleDown(m_ColliderRect.x + m_ColliderRect.w, 8);
	t_BottomRightTile.y = ClosestMultipleDown(m_ColliderRect.y + m_ColliderRect.h, 8);

	if (m_LastPosition->x > m_CurrPosition->x) {
		t_TopLeftTile.x = ClosestMultipleDown(m_ColliderRect.x, 8);
		t_BottomRightTile.x = ClosestMultipleDown(m_ColliderLastPos.x + m_ColliderRect.w, 8);
	}
	if (m_LastPosition->y > m_CurrPosition->y) {
		t_TopLeftTile.y = ClosestMultipleDown(m_ColliderRect.y, 8);
		t_BottomRightTile.y = ClosestMultipleDown(m_ColliderLastPos.y + m_ColliderRect.h, 8);
	}

	return std::make_tuple(t_TopLeftTile, t_BottomRightTile);
}

void Collider::GetCollidedTiles(const std::tuple<Vector2d, Vector2d>& p_RangeOfCollidableTiles, const float& p_DeltaTime) {
	const Vector2d& t_TopLeft = std::get<0>(p_RangeOfCollidableTiles);
	const Vector2d& t_BottomRight = std::get<1>(p_RangeOfCollidableTiles);

	auto CheckCollisionWithTile = [&](const SDL_Rect& p_TileToCheck) {
		if (m_ColliderMap[(p_TileToCheck.x / 8) + (m_MapWidth * p_TileToCheck.y / 8)] != 0) {
			SDL_FRect t_ColliderFRect = { m_ColliderRect.x, m_ColliderRect.y, m_ColliderRect.w, m_ColliderRect.h };

			if (RectUtil::DynamicRectIntersectRect(t_ColliderFRect, p_TileToCheck, *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
				m_CollidedTiles.push_back(CollidedTile{ p_TileToCheck, t_TimeHitNear, t_ContactNormal });
			}
		}
	};

	m_CollidedTiles.clear();
	for (int i = t_TopLeft.x; i <= t_BottomRight.x; i += 8) {
		for (int j = t_TopLeft.y; j <= t_BottomRight.y; j += 8) {
			if (
				i <= 0 ||
				i >= m_MapWidth * 8 ||
				j >= m_MapHeight * 8 ||
				j <= 0
				) {
				continue;
			}

			CheckCollisionWithTile(SDL_Rect{ i, j, 8, 8 });
		}
	}
}

void Collider::SortCollidedTilesByTime() {
	auto CollidedFirst = [](const CollidedTile& p_A, const CollidedTile& p_B) {
		return p_A.m_TimeHitNear < p_B.m_TimeHitNear;
	};

	std::sort(m_CollidedTiles.begin(), m_CollidedTiles.end(), CollidedFirst);
}

bool Collider::CollisionWithMap(const float& p_DeltaTime) {
	std::tuple<Vector2d, Vector2d> t_RangeOfCollidableTiles = GetRangeOfTiles();

	GetCollidedTiles(t_RangeOfCollidableTiles, p_DeltaTime);

	if (m_CollidedTiles.empty()) {
		return false;
	}

	SortCollidedTilesByTime();

	for (int i = 0; i < m_CollidedTiles.size(); i++) {
		SDL_FRect t_ColliderFRect = { m_ColliderRect.x, m_ColliderRect.y, m_ColliderRect.w, m_ColliderRect.h };

		if (RectUtil::DynamicRectIntersectRect(t_ColliderFRect, m_CollidedTiles[i].m_Tile, *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
			ResolveMapCollision(t_ContactNormal, m_CollidedTiles[i].m_Tile, t_TimeHitNear);
			if (t_ContactNormal.y == -1) {
				*m_Jumping = false;
			}
		}
	}

	return true;
}

void Collider::ResolveMapCollision(const Vector2d& p_ContactNormal, const SDL_Rect& p_Tile, const float& p_TimeHitNear) {
	if (p_ContactNormal.x) {
		//Left
		if (p_ContactNormal.x > 0) {
			m_CurrPosition->x = p_Tile.x + 8 - m_ColliderOffset.x;
		}
		//Right
		else {
			m_CurrPosition->x = p_Tile.x - m_ColliderRect.w - m_ColliderOffset.x;
		}
		m_CurrVelocity->x = 0;
		m_ColliderRect.x = m_CurrPosition->x + m_ColliderOffset.x;
	}
	if (p_ContactNormal.y) {
		//Top
		if (p_ContactNormal.y > 0) {
			m_CurrPosition->y = p_Tile.y + 8 - m_ColliderOffset.y;
		}
		//Bottom
		else {
			m_CurrPosition->y = p_Tile.y - m_ColliderRect.h - m_ColliderOffset.y;
		}
		m_CurrVelocity->y = 0;
		m_ColliderRect.y = m_CurrPosition->y + m_ColliderOffset.y;
	}
}
