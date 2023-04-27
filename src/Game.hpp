#pragma once
#include "SDL.h"
#include "Tilemap.hpp"
#include "Player.hpp"

class Game {
public:
	Game(); 
	~Game();

	virtual void OnUserCreate();
	virtual void OnUserUpdate(); 

	void Init(const char* p_Title, const char* p_Iconpath, const int& p_x, const int& p_y, int p_Width, int p_Height); 
	void HandleEvents();
	void Update();
	void Render();
	void Clean();
	inline bool Exit() { return m_IsRunning; }

	Tilemap* Level_1;
	Player* m_Player;
	Collider* m_PlayerCollider;
	SDL_Renderer* m_Renderer;

	float m_FrameDelta;

private:

	bool m_IsRunning;
	int m_w, m_h;
	SDL_Window* m_Window;
	SDL_DisplayMode m_Mode;
	SDL_Event m_Event;

	bool InFocus();
};