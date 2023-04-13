#pragma once
#include "SDL.h"
#include "Tilemap.hpp"

class Game {
public:
	Game(const Game&) = delete;

	static Game& Get() {
		static Game Instance;
		return Instance;
	}

	void Init(const char* p_Title, const char* p_Iconpath, const int& p_x, const int& p_y, int p_Width, int p_Height);
	void HandleEvents();
	void Update();
	void Render();
	void Clean();
	inline bool Exit() { return m_IsRunning; }

private:
	Game();
	~Game();

	bool m_IsRunning;
	float m_FrameDelta;
	int m_w, m_h;
	SDL_Window* m_Window;
	SDL_Renderer* m_Renderer;
	SDL_DisplayMode m_Mode;
	SDL_Event m_Event;

	Tilemap* Level_1;

	bool InFocus();
};