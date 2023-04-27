#include "Player.hpp"
#include "math/Rectangle.hpp"
#include "TextureLoader.hpp"
#include <iostream>

Player::Player(const char* p_TexPath, const SDL_Rect& p_SrcRect, const SDL_Rect& p_DestRect, SDL_Renderer* p_Renderer, const int& p_MovementSpeed) 
	: m_StrafeVelocity(p_MovementSpeed), m_Gravity(360) {
	m_Sprite = TextureUtil::LoadTexture(p_TexPath, p_Renderer);
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);

	m_Renderer = p_Renderer;
	m_SrcRect = p_SrcRect;
	m_DestRect = p_DestRect;

	m_CurrPosition.x = m_DestRect.x;
	m_CurrPosition.y = m_DestRect.y;
	m_CurrVelocity.x = 0;
	m_CurrVelocity.y = 0;

	m_Jumping = true;
}

void Player::Render() {
	//Setting Render Target to the buffer
	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	SDL_RenderClear(m_Renderer);

	//Set the destination rectangle to have current position
	m_DestRect.x = m_CurrPosition.x; 
	m_DestRect.y = m_CurrPosition.y;

	//Render the sprite onto the buffer
	SDL_RenderCopy(m_Renderer, m_Sprite, &m_SrcRect, &m_DestRect);
	SDL_SetRenderTarget(m_Renderer, NULL); 

	//Render the Buffer
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
	SDL_SetRenderTarget(m_Renderer, NULL);
}

void Player::LinkCollider(Collider* p_Collider) {
	p_Collider->LinkObject(&m_CurrVelocity, &m_CurrPosition, &m_LastPosition, &m_Jumping);
}

void Player::Update(const float& p_DeltaTime) {
	//m_CurrVelocity.Normalize();
	//m_CurrVelocity.Scale(m_StrafeVelocity);
	if(m_Jumping) m_CurrVelocity.y += m_Gravity * (p_DeltaTime / (float)1000);
	std::cout << m_CurrVelocity.y << std::endl;

	//Save Position into last position before updating
	m_LastPosition.x = m_CurrPosition.x;
	m_LastPosition.y = m_CurrPosition.y;

	//Set New positions by applying velocity
	m_CurrPosition.y += m_CurrVelocity.y * (p_DeltaTime / (float)1000);
	m_CurrPosition.x += m_CurrVelocity.x * (p_DeltaTime / (float)1000);
}

void Player::HandleEvents(const SDL_Event& p_Event) {
	const Uint8* Keys = SDL_GetKeyboardState(NULL);

	if (Keys[SDL_SCANCODE_W] && !m_Jumping) {
		m_CurrVelocity.y = -140;
		m_Jumping = true;
	}
	if (Keys[SDL_SCANCODE_S]) {
		m_CurrVelocity.y = m_StrafeVelocity;
	}
	if (Keys[SDL_SCANCODE_A]) {
		m_CurrVelocity.x = -m_StrafeVelocity;
	}
	if (Keys[SDL_SCANCODE_D]) {
		m_CurrVelocity.x = m_StrafeVelocity;
	}

	if (p_Event.type == SDL_KEYUP) {
		auto Key = p_Event.key.keysym.sym;

		switch (Key) {
		case SDLK_s:
			m_CurrVelocity.y = 0;
			break;
		case SDLK_d:
			if (Keys[SDL_SCANCODE_A]) {
				m_CurrVelocity.x = -m_StrafeVelocity;
				return;
			}
			m_CurrVelocity.x = 0;
			break;
		case SDLK_a:
			if (Keys[SDL_SCANCODE_D]) {
				m_CurrVelocity.x = m_StrafeVelocity;
				return;
			}
			m_CurrVelocity.x = 0;
			break;
		default:
			break;
		}
	}
}
