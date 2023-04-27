#include "Collider.hpp"
#include "../math/Rectangle.hpp"
#include <iostream>
#include <algorithm>

Collider::Collider(const SDL_Rect& p_ColliderRect, const int& p_OffsetX, const int& p_OffsetY, SDL_Renderer* p_Renderer) {
	m_ColliderRect = p_ColliderRect;
	m_Offset.x = p_OffsetX;
	m_Offset.y = p_OffsetY;

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

	if (p_ColliderMap == nullptr) {
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
	m_ColliderLastPos.x = m_LastPosition->x + m_Offset.x;
	m_ColliderLastPos.y = m_LastPosition->y + m_Offset.y;
	m_ColliderRect.x = m_CurrPosition->x + m_Offset.x;
	m_ColliderRect.y = m_CurrPosition->y + m_Offset.y; 

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

bool CompareCollidedTiles(const Collider::CollidedTile& p_A, const Collider::CollidedTile& p_B) {
	return p_A.m_TimeHitNear < p_B.m_TimeHitNear;
}

void Collider::CollisionWithMap(const float& p_DeltaTime) {
	*m_Jumping = true;
	SDL_Rect t_TileToCheck; 
	Vector2d t_TopLeft, t_BottomRight; 
	t_TopLeft.x = ClosestMultipleDown(m_ColliderLastPos.x, 8);
	t_BottomRight.x = ClosestMultipleDown(m_ColliderRect.x + m_ColliderRect.w, 8);
	t_TopLeft.y = ClosestMultipleDown(m_ColliderLastPos.y, 8); 
	t_BottomRight.y = ClosestMultipleDown(m_ColliderRect.y + m_ColliderRect.h, 8); 

	if (m_LastPosition->x > m_CurrPosition->x) { 
		t_TopLeft.x = ClosestMultipleDown(m_ColliderRect.x, 8); 
		t_BottomRight.x = ClosestMultipleDown(m_ColliderLastPos.x + m_ColliderRect.w, 8); 
	}
	if (m_LastPosition->y > m_CurrPosition->y) {
		t_TopLeft.y = ClosestMultipleDown(m_ColliderRect.y, 8); 
		t_BottomRight.y = ClosestMultipleDown(m_ColliderLastPos.y + m_ColliderRect.h, 8); 
	}

	m_TilesToCollide.clear();
	for (int i = t_TopLeft.x; i <= t_BottomRight.x; i = i + 8) { 
		for (int j = t_TopLeft.y; j <= t_BottomRight.y; j = j + 8) { 
			t_TileToCheck = { i, j, 8, 8 }; 
			m_TilesToCollide.push_back(t_TileToCheck); 
		}
	}

	CollidedTile t_Tile;
	Vector2d t_ContactPoint, t_ContactNormal;
	double t_TimeHitNear;

	m_TilesCollided.clear();
	for (int i = 0; i < m_TilesToCollide.size(); i++) {
		if (m_ColliderMap[m_TilesToCollide[i].x / 8 + (40 * m_TilesToCollide[i].y / 8)] == 1) {
			SDL_FRect t_ColliderFRect = { m_ColliderRect.x, m_ColliderRect.y, m_ColliderRect.w, m_ColliderRect.h }; 

			if (RectUtil::DynamicRectIntersectRect(t_ColliderFRect, m_TilesToCollide[i], *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
				t_Tile = { m_TilesToCollide[i], t_TimeHitNear, t_ContactNormal };
				m_TilesCollided.push_back(t_Tile); 
			}
		}
	}
	
	if (m_TilesCollided.empty()) {
		return;
	}

	std::sort(m_TilesCollided.begin(), m_TilesCollided.end(), CompareCollidedTiles);

	for (int i = 0; i < m_TilesCollided.size(); i++) {
		SDL_FRect t_ColliderFRect = { m_ColliderRect.x, m_ColliderRect.y, m_ColliderRect.w, m_ColliderRect.h };

		if (RectUtil::DynamicRectIntersectRect(t_ColliderFRect, m_TilesCollided[i].m_Tile, *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
			ResolveMapCollision(t_ContactNormal, m_TilesCollided[i].m_Tile, t_TimeHitNear);
			if (t_ContactNormal.y == -1) {
				*m_Jumping = false;
			}
		}
	}
}

int Collider::ClosestMultipleDown(const float& p_X, const int& p_N) {
	return (int)p_X - ((int)p_X % p_N);	
}

void Collider::ResolveMapCollision(const Vector2d& p_ContactNormal, const SDL_Rect& p_Tile, const float& p_TimeHitNear) {
	if (p_ContactNormal.x) {
		//Left
		if (p_ContactNormal.x > 0) {
			m_CurrPosition->x = p_Tile.x + 8 - m_Offset.x;
		}
		//Right
		else {
			m_CurrPosition->x = p_Tile.x - m_ColliderRect.w - m_Offset.x;
		}
		m_CurrVelocity->x = 0;
		m_ColliderRect.x = m_CurrPosition->x + m_Offset.x;
	}
	if (p_ContactNormal.y) {
		//Top
		if (p_ContactNormal.y > 0) {
			m_CurrPosition->y = p_Tile.y + 8 - m_Offset.y;
		}
		//Bottom
		else {
			m_CurrPosition->y = p_Tile.y - m_ColliderRect.h - m_Offset.y;
		}
		m_CurrVelocity->y = 0;
		m_ColliderRect.y = m_CurrPosition->y + m_Offset.y;
	}
}
