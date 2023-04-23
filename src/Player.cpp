#include "Player.hpp"
#include "math/Rectangle.hpp"
#include "TextureLoader.hpp"
#include <iostream>

Player::Player(const char* p_TexPath, const SDL_Rect& p_SrcRect, const SDL_Rect& p_DestRect, SDL_Renderer* p_Renderer) 
	: m_StrafeVelocity(160), m_Gravity(140) {
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

	m_AllowDown = true;
	m_AllowUp = true;
	m_AllowLeft = true;
	m_AllowRight = true;
}

void Player::Render() {
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
}

void Player::Update(unsigned short* p_Collider, const int& p_ColliderWidth, const float& p_DeltaTime) {
	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	SDL_RenderClear(m_Renderer);

	//m_CurrVelocity.y += m_Gravity * (p_DeltaTime / (float)1000);
	m_CurrVelocity.Normalize();
	m_CurrVelocity.Scale(m_StrafeVelocity);

	m_y += m_CurrVelocity.y * (p_DeltaTime / (float)1000);
	m_x += m_CurrVelocity.x * (p_DeltaTime / (float)1000);

	CheckCollisions(p_Collider, p_ColliderWidth);
	
	m_DestRect.x = m_x;
	m_DestRect.y = m_y;

	SDL_RenderCopy(m_Renderer, m_Sprite, &m_SrcRect, &m_DestRect);

	SDL_SetRenderTarget(m_Renderer, NULL);
}

void Player::HandleEvents(const SDL_Event& p_Event) {
	const Uint8* Keys = SDL_GetKeyboardState(NULL);

	if (Keys[SDL_SCANCODE_W]) {
		if (m_AllowUp) {
			m_CurrVelocity.y = -m_StrafeVelocity;
		}
		else {
			m_CurrVelocity.y = 0;
		}
	}
	if (Keys[SDL_SCANCODE_S]) {
		if (m_AllowDown) {
			m_CurrVelocity.y = m_StrafeVelocity;
		}
		else {
			m_CurrVelocity.y = 0;
		}
	}
	if (Keys[SDL_SCANCODE_A]) {
		if (m_AllowLeft) {
			m_CurrVelocity.x = -m_StrafeVelocity;
		}
		else {
			m_CurrVelocity.x = 0;
		}
	}
	if (Keys[SDL_SCANCODE_D]) {
		if (m_AllowRight) {
			m_CurrVelocity.x = m_StrafeVelocity;
		}
		else {
			m_CurrVelocity.x = 0;
		}
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

void Player::CheckCollisions(unsigned short* p_Collider, const int& p_ColliderWidth) {
	SDL_Rect TempRect;
	m_AllowDown = true;
	m_AllowUp = true;
	m_AllowLeft = true;
	m_AllowRight = true;

	int t_Top = 8 * (int)(m_y / 8);
	int t_Bottom = 8 * (int)((m_y + m_DestRect.h - 1) / 8);
	int t_Left = 8 * (int)(m_x / 8);
	int t_Right = 8 * (int)((m_x + m_DestRect.w - 1) / 8);

	//left
	int i = 0;
	while ((t_Top + i) <= m_y + m_DestRect.h - 1) {
		TempRect = { t_Left - 8, t_Top + i, 8, 8 };
		if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && m_x <= TempRect.x + 9) {
			m_AllowLeft = false;
			SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100);
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		else {
			SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100); 
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		i += 8;
	}
	//right
	i = 0;
	while ((t_Top + i) <= m_y + m_DestRect.h - 1) {
		TempRect = { t_Right + 8, t_Top + i, 8, 8 }; 
		if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && m_x + m_DestRect.w >= TempRect.x) {
			m_AllowRight = false;
			SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100);
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		else {
			SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100);
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		i += 8;
	}
	//top
	i = 0;
	while ((t_Left + i) <= m_x + m_DestRect.w - 1) {
		TempRect = { t_Left + i, t_Top - 8, 8, 8 };
		if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && m_y <= TempRect.y + 9) {
			m_AllowUp = false;
			SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100);
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		else {
			SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100);
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		i += 8;
	}
	//bottom
	i = 0;
	while ((t_Left + i) <= m_x + m_DestRect.w - 1) { 
		TempRect = { t_Left + i, t_Bottom + 8, 8, 8 };
		if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && m_y + m_DestRect.h >= TempRect.y) {
			m_AllowDown = false;
			SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100);
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		else {
			SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100);
			SDL_RenderFillRect(m_Renderer, &TempRect);
		}
		i += 8;
	}

	if (!(m_AllowDown && m_AllowUp && m_AllowLeft && m_AllowRight)) {
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		return;
	}
	//Diagonal Bottom Left
	TempRect = { 8 * (int)(m_x / 8) - 8, 8 * (int)((m_y + m_DestRect.h - 1) / 8) + 8, 8, 8 };
	if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && (m_y + m_DestRect.h >= TempRect.y) && (m_x <= TempRect.x + 9)) {
		m_AllowDown = false;
		SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100);
		SDL_RenderFillRect(m_Renderer, &TempRect);
	}
	else {
		SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100); 
		SDL_RenderFillRect(m_Renderer, &TempRect); 
	}
	//Diagonal Bottom Right
	TempRect = { 8 * (int)((m_x + m_DestRect.w - 1) / 8) + 8, 8 * (int)((m_y + m_DestRect.h - 1) / 8) + 8, 8, 8 };
	if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && (m_y + m_DestRect.h >= TempRect.y) && (m_x + m_DestRect.w >= TempRect.x)) {
		m_AllowDown = false;
		SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100); 
		SDL_RenderFillRect(m_Renderer, &TempRect); 
	}
	else {
		SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100); 
		SDL_RenderFillRect(m_Renderer, &TempRect); 
	}
	//Diagonal Top Left
	TempRect = { 8 * (int)(m_x / 8) - 8, 8 * (int)(m_y / 8) - 8, 8, 8 };
	if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && (m_x <= TempRect.x + 9) && (m_y <= TempRect.y + 9)) {
		m_AllowLeft = false;
		SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100);
		SDL_RenderFillRect(m_Renderer, &TempRect);
	}
	else {
		SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100);
		SDL_RenderFillRect(m_Renderer, &TempRect);
	}
	//Diagonal Top Right
	TempRect = { 8 * (int)((m_x + m_DestRect.w - 1) / 8) + 8, 8 * (int)(m_y / 8) - 8, 8, 8 };
	if (p_Collider[(TempRect.x / 8) + (TempRect.y * p_ColliderWidth / 8)] == 1 && (m_x + m_DestRect.w >= TempRect.x) && (m_y <= TempRect.y + 9)) {
		m_AllowRight = false;
		SDL_SetRenderDrawColor(m_Renderer, 0, 255, 0, 100);
		SDL_RenderFillRect(m_Renderer, &TempRect);
	}
	else {
		SDL_SetRenderDrawColor(m_Renderer, 255, 0, 0, 100); 
		SDL_RenderFillRect(m_Renderer, &TempRect); 
	}
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
}
