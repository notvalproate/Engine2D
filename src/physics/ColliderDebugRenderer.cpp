#include "Collider.hpp"

namespace notval {

	ColliderDebugRenderer::ColliderDebugRenderer(SDL_Renderer* renderer) : m_Renderer(renderer), m_CollisionLayer(nullptr) {
		m_Buffer = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1280, 720);
		m_BufferRect = { 0, 0, 1280, 720 };
		SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
	}

	void ColliderDebugRenderer::SetCollisionLayer(const Tilemap::Layer* collider) {
		m_CollisionLayer = collider;

		int bufferWidth = m_CollisionLayer->width * 32;
		int bufferHeight = m_CollisionLayer->height * 32;

		m_Buffer = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, bufferWidth, bufferHeight);
		m_BufferRect = { 0, 0, bufferWidth, bufferHeight };
		SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
	}

	void ColliderDebugRenderer::DebugRender(const DynamicCollider2D& collider, const float deltaTime, const std::unique_ptr<Camera>& camera) {
		//Debug to render hitboxes
		SDL_SetRenderTarget(m_Renderer, m_Buffer);
		SDL_RenderClear(m_Renderer);

		//Debug to render all the map colliders
		SDL_Rect tile;
		if (collider.m_CollidesWithMap && m_CollisionLayer != nullptr) {
			int size = m_CollisionLayer->height * m_CollisionLayer->width;
			SDL_SetRenderDrawColor(m_Renderer, 0, 0, 255, 100);
			for (int i = 0; i < size; i++) {
				if (m_CollisionLayer->data[i] != 0) {
					tile = { collider.m_TileSize * (i % m_CollisionLayer->width), collider.m_TileSize * (i / m_CollisionLayer->width), collider.m_TileSize, collider.m_TileSize };
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

		camera->RenderToBuffer(m_Buffer, NULL, &m_BufferRect);
	}

}