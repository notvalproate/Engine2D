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

	virtual void OnUserCreate() {};
	virtual void OnUserEvents() {};
	virtual void OnUserUpdate() {};
	virtual void OnUserRender() {};

	void Init(const char* p_Title, const char* p_Iconpath, const int& p_x, const int& p_y, int p_Width, int p_Height); 
	void HandleEvents();
	void Update();
	void Render();

	inline bool isRunning() const { return m_IsRunning; }

protected:
	std::unique_ptr<Tilemap> m_Level;
	std::unique_ptr<Player> m_Player;
	std::unique_ptr<DynamicCollider2D> m_PlayerCollider;
	std::unique_ptr<ColliderDebugRenderer> m_ColliderDebugger;

	SDL_Renderer* m_Renderer;

	float m_DeltaTime{};

private:
	bool m_IsRunning{};
	int m_Width{}, m_Height{};
	SDL_Window* m_Window;
	SDL_DisplayMode m_Mode;
	SDL_Event m_Event;

	inline bool InFocus() const { return (SDL_GetWindowFlags(m_Window) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS)); };
};