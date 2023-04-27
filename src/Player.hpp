#pragma once
#include "SDL.h"
#include "math/Vector2d.h"
#include <vector>
#include "physics/Collider.hpp"

class Player {
public:
	Player(const char* p_TexPath, const SDL_Rect& p_SrcRect, const SDL_Rect& p_DestRect, SDL_Renderer* p_Renderer, const int& p_MovementSpeed);
	~Player();
	
	
	void LinkCollider(Collider* p_Collider);

	void HandleEvents(const SDL_Event& p_Event);
	void Update(const float& p_DeltaTime);
	void Render();

	SDL_Texture* GetBuffer() { return m_Buffer; }
private:
	const int m_StrafeVelocity;
	const int m_Gravity;

	Vector2d m_CurrPosition, m_LastPosition;
	Vector2d m_CurrVelocity;

	SDL_Rect m_SrcRect, m_DestRect;
	SDL_Texture *m_Sprite, *m_Buffer;
	SDL_Renderer* m_Renderer;

	bool m_Jumping;
};