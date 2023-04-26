#include "Collider.hpp"
#include "../math/Rectangle.hpp"
#include <iostream>

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

	r = 255;
	g = 165;
	b = 0;
}

Collider::~Collider() { }

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

void Collider::LinkObject(Vector2d* p_CurrVelocity, Vector2d* p_CurrPosition, Vector2d* p_LastPosition) {
	m_CurrPosition = p_CurrPosition; 
	m_LastPosition = p_LastPosition;
	m_CurrVelocity = p_CurrVelocity;
}

void Collider::Update(const float& p_DeltaTime) {
	m_ColliderRect.x = m_CurrPosition->x + m_Offset.x;
	m_ColliderRect.y = m_CurrPosition->y + m_Offset.y; 

	if (m_CollidesWithMap) {
		CollisionWithMap(p_DeltaTime);
	}
}

void Collider::DebugRender(const float& p_DeltaTime) {
	//Debug to render hitbox
	SDL_SetRenderTarget(m_Renderer, m_Buffer); 
	SDL_RenderClear(m_Renderer);

	//Checking working 
	Vector2d t_ContactPoint, t_ContactNormal;
	double t_TimeHitNear;
	SDL_Rect TestRect = {70, 70, 8, 8}; 
	SDL_Rect Contact; 
	SDL_FRect TempRect = { m_ColliderRect.x, m_ColliderRect.y, m_ColliderRect.w, m_ColliderRect.h };

	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 255, 255);
	if (RectUtil::DynamicRectIntersectRect(TempRect, TestRect, *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
		SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 255); 
		SDL_RenderFillRect(m_Renderer, &TestRect); 
		SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
		Contact = { (int)t_ContactPoint.x, (int)t_ContactPoint.y, 3, 3 }; 
		SDL_RenderFillRect(m_Renderer, &Contact);;
	}
	else {
		SDL_RenderFillRect(m_Renderer, &TestRect);
	}

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

	SDL_SetRenderDrawColor(m_Renderer, r, g, b, 100);
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

void Collider::CollisionWithMap(const float& p_DeltaTime) {
	SDL_Rect t_Tile;
	Vector2d t_ContactPoint, t_ContactNormal;

	double t_TimeHitNear;

	for (int i = 0; i <= m_MapHeight; i++) {
		for (int j = 0; j <= m_MapWidth; j++) {
			if (m_ColliderMap[m_MapWidth * i + j] == 1) {
				t_Tile = { j * 8, i * 8, 8, 8 };
				SDL_FRect t_ColliderFRect = { m_ColliderRect.x, m_ColliderRect.y, m_ColliderRect.w, m_ColliderRect.h }; 

				if (RectUtil::DynamicRectIntersectRect(t_ColliderFRect, t_Tile, *m_CurrVelocity, t_ContactPoint, t_ContactNormal, t_TimeHitNear, p_DeltaTime)) {
					ResolveMapCollision(t_ContactNormal, t_Tile); 
				}
			}
		}
	}
}

void Collider::ResolveMapCollision(const Vector2d& p_ContactNormal, const SDL_Rect& p_Tile) {

	std::cout << p_Tile.x << " " << p_Tile.y << std::endl;
	if (p_ContactNormal.x) {
		//Left
		if (p_ContactNormal.x > 0) {
			m_CurrPosition->x = p_Tile.x + 8 - m_Offset.x;
			m_ColliderRect.x = m_CurrPosition->x + m_Offset.x;
		}
		//Right
		else {
			m_CurrPosition->x = p_Tile.x - m_ColliderRect.w - m_Offset.x;
			m_ColliderRect.x = m_CurrPosition->x + m_Offset.x;
		}
	}
	if (p_ContactNormal.y) {
		//Top
		if (p_ContactNormal.y > 0) {
			m_CurrPosition->y = p_Tile.y + 8 - m_Offset.y;
			m_ColliderRect.y = m_CurrPosition->y + m_Offset.y;
		}
		//Bottom
		else {
			m_CurrPosition->y = p_Tile.y - m_ColliderRect.h - m_Offset.y;
			m_ColliderRect.y = m_CurrPosition->y + m_Offset.y;
		}
	}
}

void Collider::CollisionWith(const Collider& p_OtherCollider) {

}

void Collider::ResolveCollision(const Collider& p_OtherCollider) {

}
