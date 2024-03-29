#include "Player.hpp"
#include "math/Rectangle.hpp"
#include "TextureLoader.hpp"
#include <iostream>

namespace notval {

	Player::Player(SDL_Renderer* renderer, const char* texPath, const SDL_Rect& srcRect, const int movementSpeed, const int jumpStrength, const int gravity)
		: m_MovementSpeed(movementSpeed), m_JumpStrength(jumpStrength), m_Gravity(gravity), m_Renderer(renderer), m_SrcRect(srcRect), m_DestRect(srcRect), m_Jumping(true)
	{
		m_Sprite = TextureUtil::LoadTexture(texPath, renderer);
		m_CurrPosition.x = 180;
		m_CurrPosition.y = 50;
	}

	Player::~Player() {
		SDL_DestroyTexture(m_Sprite);
	}

	void Player::HandleEvents(const SDL_Event& event) {
		//Primitive Input handler for now since no player controller class yet
		const Uint8* Keys = SDL_GetKeyboardState(NULL);

		//Checking for whether keys are held down or not
		if ((Keys[SDL_SCANCODE_W] || Keys[SDL_SCANCODE_SPACE]) && !m_Jumping) {
			m_CurrVelocity.y = -m_JumpStrength;
			m_Jumping = true;
		}
		if (Keys[SDL_SCANCODE_A]) {
			m_CurrVelocity.x = -m_MovementSpeed;
		}
		if (Keys[SDL_SCANCODE_D]) {
			m_CurrVelocity.x = m_MovementSpeed;
		}

		if (event.type == SDL_KEYUP) {
			auto Key = event.key.keysym.sym;

			switch (Key) {
			case SDLK_d:
				//If the opposite direction key is still pressed while it is left up, continue in the other direction
				if (Keys[SDL_SCANCODE_A]) {
					m_CurrVelocity.x = -m_MovementSpeed;
					return;
				}
				m_CurrVelocity.x = 0;
				break;
			case SDLK_a:
				if (Keys[SDL_SCANCODE_D]) {
					m_CurrVelocity.x = m_MovementSpeed;
					return;
				}
				m_CurrVelocity.x = 0;
				break;
			}
		}
	}


	void Player::Update(const float deltaTime) {
		//Apply gravity only while airborne
		m_CurrVelocity.y += m_Gravity * deltaTime;

		m_LastPosition = m_CurrPosition;
		m_CurrPosition += m_CurrVelocity * deltaTime;
	}

	void Player::Render(const std::unique_ptr<Camera>& camera) {
		m_DestRect.x = (int)m_CurrPosition.x;
		m_DestRect.y = (int)m_CurrPosition.y;
		camera->RenderToBuffer(m_Sprite, &m_SrcRect, &m_DestRect);
	}

}