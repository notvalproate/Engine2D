#include "Player.hpp"
#include "math/Rectangle.hpp"
#include "TextureLoader.hpp"
#include <iostream>

Player::Player(const char* p_TexPath, const SDL_Rect& p_SrcRect, const SDL_Rect& p_DestRect, SDL_Renderer* p_Renderer) 
	: m_StrafeVelocity(80), m_Gravity(140) {
	m_Sprite = TextureUtil::LoadTexture(p_TexPath, p_Renderer);
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);

	m_Renderer = p_Renderer;
	m_SrcRect = p_SrcRect;
	m_DestRect = p_DestRect;

	m_x = m_DestRect.x;
	m_y = m_DestRect.y;
	m_CurrVelocity.x = 0;
	m_CurrVelocity.y = 0;
	ip = 0;
}

void Player::Render() {
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
}

void Player::Update(unsigned short* p_Collider, const float& p_DeltaTime) {
	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	SDL_RenderClear(m_Renderer);

	//m_CurrVelocity.y += m_Gravity * (p_DeltaTime / (float)1000);
	m_y += m_CurrVelocity.y * (p_DeltaTime / (float)1000);
	m_x += m_CurrVelocity.x * (p_DeltaTime / (float)1000);

	CheckCollisions(p_Collider);
	
	m_DestRect.x = m_x;
	m_DestRect.y = m_y;

	SDL_RenderCopy(m_Renderer, m_Sprite, &m_SrcRect, &m_DestRect);

	SDL_SetRenderTarget(m_Renderer, NULL);
}

void Player::HandleEvents(const SDL_Event& p_Event) {
	const Uint8* Keys = SDL_GetKeyboardState(NULL);

	if (Keys[SDL_SCANCODE_W]) {
		m_CurrVelocity.y = -m_StrafeVelocity;
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
		case SDLK_w:
			m_CurrVelocity.y = 0;
			break;
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

void Player::CheckCollisions(unsigned short* p_Collider) {
	
}
