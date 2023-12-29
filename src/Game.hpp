#pragma once
#include "SDL.h"
#include "tilemap/Tilemap.hpp"
#include "Player.hpp"
#include "physics/Collider.hpp"
#include <memory>

class Game {
public:
	Game(); 
	virtual ~Game();

	Game(const Game& other) = delete;
	Game(const Game&& other) = delete;

	Game operator=(const Game& other) = delete;
	Game operator=(const Game&& other) = delete;

	virtual void OnUserCreate() = 0;
	virtual void OnUserUpdate() = 0; 
	virtual void OnUserRender() = 0; 

	void Init(const char* p_Title, const char* p_Iconpath, const int& p_x, const int& p_y, int p_Width, int p_Height); 
	void HandleEvents();
	void Update();
	void Render();
	inline bool Exit() const { return m_IsRunning; }

protected:
	std::unique_ptr<Tilemap> m_Level;
	std::unique_ptr<Player> m_Player;
	std::unique_ptr<DynamicCollider2D> m_PlayerCollider;
	std::unique_ptr<ColliderDebugRenderer> m_ColliderDebugger;

	SDL_Renderer* m_Renderer;

	float m_DeltaTime;

private:
	bool m_IsRunning;
	int m_w{}, m_h{};
	SDL_Window* m_Window;
	SDL_DisplayMode m_Mode;
	SDL_Event m_Event;

	bool InFocus();
};