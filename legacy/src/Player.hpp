#pragma once
#include "SDL.h"
#include "Camera.hpp"
#include "math/Vector2d.h"
#include <vector>
#include <memory>

// Forward Declaration to be friend
namespace notval {

	class DynamicCollider2D;

	class Player {
	public:
		Player(SDL_Renderer* renderer, const char* texPath, const SDL_Rect& srcRect, const int movementSpeed, const int jumpStrength, const int gravity);
		~Player();

		Player(const Player& other) = delete;
		Player(const Player&& other) = delete;

		Player& operator=(const Player& other) = delete;
		Player& operator=(const Player&& other) = delete;

		void HandleEvents(const SDL_Event& event);
		void Update(const float deltaTime);
		void Render(const std::unique_ptr<Camera>& camera);
	private:
		const int m_MovementSpeed, m_JumpStrength, m_Gravity;

		Vector2d m_CurrVelocity, m_CurrPosition, m_LastPosition;

		SDL_Rect m_SrcRect, m_DestRect;
		SDL_Texture* m_Sprite;

		SDL_Renderer* m_Renderer;

		bool m_Jumping;

		friend DynamicCollider2D;
	};

}