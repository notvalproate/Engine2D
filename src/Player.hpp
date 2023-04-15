#pragma once
#include "SDL.h"

class Player {
public:
	Player(const Player&) = delete;

	static Player& Get() {
		static Player Instance;
		return Instance;
	}

	void Init(const char* m_TexPath, const SDL_Rect& p_SrcRect, const SDL_Rect& p_DestRect);
	void Render();
	void Update();
	void HandleEvents();

private:
	Player();
	~Player();

	SDL_Rect m_SrcRect, m_DestRect;
};