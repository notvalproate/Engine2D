#include "Game.hpp"
#include "SDL_image.h"
#include "physics/Collider.hpp"
#include <iostream>
#include <vector>

Game::Game() : m_IsRunning(false), m_DeltaTime(16), m_Window(nullptr), m_Renderer(nullptr) { }

Game::~Game() {
	SDL_DestroyWindow(m_Window);
	SDL_DestroyRenderer(m_Renderer);
	SDL_Quit();
	std::cout << "Game Cleaned" << std::endl;
}

void Game::Init(const char* p_Title, const char* p_Iconpath, const int& p_x, const int& p_y, int p_Width, int p_Height) {
	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		std::cout << "Error: Couldn't Initialize Subsystems..." << std::endl;
		return;
	}
	std::cout << "Stage: Initialized Subsystems..." << std::endl;

	//Get the user's display mode
	if (SDL_GetDisplayMode(0, 0, &m_Mode)) {
		std::cout << "Error: Couldn't Get Display Mode...Framerate set to 60" << std::endl;
	}
	else {
		std::cout << "Stage: Display Mode Initialized..." << std::endl;
		m_DeltaTime = (float)1.0 / (float)m_Mode.refresh_rate;
	}

	m_Width = p_Width;
	m_Height = p_Height;

	//Create an SDL Window
	if (!(m_Window = SDL_CreateWindow(p_Title, p_x, p_y, p_Width, p_Height, 0))) {
		std::cout << "Error: Couldn't Initialize Window..." << std::endl;
		return;
	}

	//Setting Hints
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	SDL_Surface* TempSurface = IMG_Load(p_Iconpath);
	SDL_SetWindowIcon(m_Window, TempSurface); //Setting window icon
	SDL_FreeSurface(TempSurface);
	std::cout << "Stage: Initialized Window..." << std::endl;

	//Create an SDL Renderer
	if (!(m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE))) {
		std::cout << "Error: Couldn't Initialize Renderer..." << std::endl;
		return;
	}
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	std::cout << "Stage: Initialized Renderer..." << std::endl;

	m_IsRunning = true; //Boolean to check if window close

	// Collider Debugger

	m_ColliderDebugger = std::make_unique<ColliderDebugRenderer>(m_Renderer);

	OnUserCreate();
}

void Game::HandleEvents() {
	SDL_PollEvent(&m_Event);
	if (m_Event.type == SDL_QUIT) {
		m_IsRunning = false;
		return;
	}
	if (m_Event.type == SDL_KEYDOWN) {
		switch (m_Event.key.keysym.sym) { //Check for which key, pause if esc, toggle fullscreen if f11
		case SDLK_F11:
			int wid, hi;
			SDL_GetWindowSize(m_Window, &wid, &hi);
			//If the window doesnt have fullscreen flag, set the window to fullscreen
			if (!(SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN)) {
				SDL_SetWindowSize(m_Window, m_Mode.w, m_Mode.h);
				SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN);
				m_Level->RenderToBuffer();
				break;
			}
			//Else remove window fullscreen flag and set resolution
			SDL_SetWindowSize(m_Window, 1280, 720);
			SDL_SetWindowFullscreen(m_Window, 0);
			m_Level->RenderToBuffer();
			break;
		}
	}
	m_Player->HandleEvents(m_Event);
}

void Game::Update() {
	m_Player->Update(m_DeltaTime);
	m_PlayerCollider->Update(m_DeltaTime);
	OnUserUpdate();
}
 
void Game::Render() {
	m_Level->Render();
	m_Player->Render();
	OnUserRender();

	SDL_RenderPresent(m_Renderer); 

	if (!InFocus()) {
		SDL_WaitEvent(NULL); //If Window not in focus, do not render and wait for window to come in focus.
	}
}