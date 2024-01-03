#include "Collider.hpp"
#include <algorithm>
#include "../math/Rectangle.hpp"

DynamicCollider2D::DynamicCollider2D(const unsigned short tileSize, const int width, const int height, const int offsetX, const int offsetY) : StaticCollider2D(width, height, 0, 0) {
	m_ColliderOffset.x = offsetX;

	m_ColliderOffset.y = offsetY;
	m_TileSize = tileSize;

	m_CurrPosition = m_CurrVelocity = m_LastPosition = nullptr;
	m_Jumping = nullptr;
}

void DynamicCollider2D::SetColliderMap(std::shared_ptr<unsigned short[]> colliderMap, const int mapWidth, const int mapHeight) {
	m_ColliderMap = colliderMap;

	if (m_ColliderMap == nullptr) {
		m_CollidesWithMap = false;
		return;
	}

	m_CollidesWithMap = true;
	m_MapWidth = mapWidth;
	m_MapHeight = mapHeight;
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

	topLeftTile.x = std::clamp((int)topLeftTile.x, 0, (m_MapWidth - 1) * m_TileSize);
	topLeftTile.y = std::clamp((int)topLeftTile.y, 0, (m_MapHeight - 1) * m_TileSize);
	bottomRightTile.x = std::clamp((int)bottomRightTile.x, 0, (m_MapWidth - 1) * m_TileSize);
	bottomRightTile.y = std::clamp((int)bottomRightTile.y, 0, (m_MapHeight - 1) * m_TileSize);

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

	if (m_ColliderMap[(tileToCheck.x / m_TileSize) + (m_MapWidth * tileToCheck.y / m_TileSize)] != 0) {
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

// COLLIDER DEBUG

ColliderDebugRenderer::ColliderDebugRenderer(SDL_Renderer* renderer) : m_Renderer(renderer) { 
	m_Buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1280, 720);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
}

void ColliderDebugRenderer::DebugRender(const DynamicCollider2D& collider, const float deltaTime, const std::unique_ptr<Camera>& camera) {
	//Debug to render hitbox
	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	SDL_RenderClear(m_Renderer);

	//Checking working 
	Vector2d contactPoint, contactNormal;
	double timeHitNear;
	SDL_Rect testRect = { 70, 70, collider.m_TileSize, collider.m_TileSize };
	SDL_Rect contact;
	SDL_FRect tempRect = { collider.m_ColliderRect.x, collider.m_ColliderRect.y, collider.m_ColliderRect.w, collider.m_ColliderRect.h };

	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 255, 255);
	if (RectUtil::DynamicRectIntersectRect(tempRect, testRect, *collider.m_CurrVelocity, contactPoint, contactNormal, timeHitNear, deltaTime)) {
		SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(m_Renderer, &testRect);
		SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 255);
		contact = { (int)contactPoint.x, (int)contactPoint.y, 3, 3 };
		SDL_RenderFillRect(m_Renderer, &contact);;
	}
	else {
		SDL_RenderFillRect(m_Renderer, &testRect);
	}

	//Debug to render all the map colliders
	SDL_Rect tile;
	if (collider.m_CollidesWithMap) {
		int size = collider.m_MapHeight * collider.m_MapWidth;
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 255, 100);
		for (int i = 0; i < size; i++) {
			if (collider.m_ColliderMap[i] == 1) {
				tile = { collider.m_TileSize * (i % collider.m_MapWidth), collider.m_TileSize * (i / collider.m_MapWidth), collider.m_TileSize, collider.m_TileSize };
				SDL_RenderFillRect(m_Renderer, &tile);
			}
		}
	}

	SDL_SetRenderDrawColor(m_Renderer, 255, 165, 0, 100);
	SDL_RenderFillRect(m_Renderer, &collider.m_ColliderRect);

	//Debug to render velocity direction
	int centerx = collider.m_ColliderRect.x + collider.m_ColliderRect.w / 2;
	int centery = collider.m_ColliderRect.y + collider.m_ColliderRect.h / 2;
	SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 255);
	SDL_RenderDrawLine(m_Renderer, centerx, centery, centerx + collider.m_TileSize * 2 * (collider.m_CurrVelocity->x / collider.m_CurrVelocity->GetMagnitude()), centery + collider.m_TileSize * 2 * (collider.m_CurrVelocity->y / collider.m_CurrVelocity->GetMagnitude()));
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);

	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	SDL_SetRenderTarget(m_Renderer, NULL);
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
}