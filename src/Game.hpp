#pragma once
#include "SDL.h"
#include "tilemap/Tilemap.hpp"
#include "Player.hpp"

class Game {
public:
	Game(); 
	virtual ~Game() = default;

	virtual void OnUserCreate() = 0;
	virtual void OnUserUpdate() = 0; 

	void Init(const char* p_Title, const char* p_Iconpath, const int& p_x, const int& p_y, int p_Width, int p_Height); 
	void HandleEvents();
	void Update();
	void Render();
	void Clean();
	inline bool Exit() const { return m_IsRunning; }

	Tilemap* Level_1;
	Player* m_Player;
	DynamicCollider2D* m_PlayerCollider;
	SDL_Renderer* m_Renderer;
	ColliderDebugRenderer* m_ColliderDebugger;

	float m_DeltaTime;
private:

	bool m_IsRunning;
	int m_w{}, m_h{};
	SDL_Window* m_Window;
	SDL_DisplayMode m_Mode;
	SDL_Event m_Event;

	bool InFocus();
};