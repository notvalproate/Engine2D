#pragma once
#include "SDL.h"
#include "math/Vector2d.h"
#include <vector>

class Player {
public:
	Player(const char* p_TexPath, const SDL_Rect& p_SrcRect, const SDL_Rect& p_DestRect, SDL_Renderer* p_Renderer);
	~Player();
	
	void Render();
	void Update(unsigned short* p_Collider, const float& p_DeltaTime);
	void HandleEvents(const SDL_Event& p_Event);
	
private:
	double m_x, m_y;
	const int m_StrafeVelocity;
	const int m_Gravity;
	int ip;

	Vector2d m_CurrVelocity;
	std::vector<unsigned int> m_InputStack;

	SDL_Rect m_SrcRect, m_DestRect;
	SDL_Texture *m_Sprite, *m_Buffer;
	SDL_Renderer* m_Renderer;

	void CheckCollisions(unsigned short* p_Collider);
};