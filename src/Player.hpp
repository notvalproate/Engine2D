#pragma once
#include "SDL.h"
#include "math/Vector2d.h"
#include <vector>

// Forward Declaration to be friend
class DynamicCollider2D;

class Player {
public:
	Player(SDL_Renderer* p_Renderer, const char* p_TexPath, const SDL_Rect& p_SrcRect, const int& p_MovementSpeed, const int& p_Gravity);
	~Player();

	Player(const Player& other) = delete;
	Player(const Player&& other) = delete;

	Player& operator=(const Player& other) = delete;
	Player& operator=(const Player&& other) = delete;

	void HandleEvents(const SDL_Event& p_Event);
	void Update(const float& p_DeltaTime);
	void Render();
private:
	const int m_MovementSpeed, m_Gravity;

	Vector2d m_CurrVelocity, m_CurrPosition, m_LastPosition;

	SDL_Rect m_SrcRect, m_DestRect;
	SDL_Texture *m_Sprite, *m_Buffer;

	SDL_Renderer *m_Renderer;

	bool m_Jumping;

	friend DynamicCollider2D;
};