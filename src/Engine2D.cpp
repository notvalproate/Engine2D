#include "Engine2D.hpp"
#include "SDL_image.h"
#include "physics/Collider.hpp"
#include <iostream>
#include <vector>

namespace notval {
	Engine2D::Engine2D() : m_IsRunning(false), m_DeltaTime(16), m_Window(nullptr), m_Renderer(nullptr) { }

	Engine2D::~Engine2D() {
		SDL_DestroyWindow(m_Window);
		SDL_DestroyRenderer(m_Renderer);
		SDL_Quit();
		std::cout << "Game Cleaned" << std::endl;
	}

	void Engine2D::Init(const char* title, const char* iconpath, const int x, const int y, int windowWidth, int windowHeight) {
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

		m_Width = windowWidth;
		m_Height = windowHeight;

		//Create an SDL Window
		if (!(m_Window = SDL_CreateWindow(title, x, y, windowWidth, windowHeight, 0))) {
			std::cout << "Error: Couldn't Initialize Window..." << std::endl;
			return;
		}

		//Setting Hints
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

		SDL_Surface* TempSurface = IMG_Load(iconpath);
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

		// Camera

		m_Camera = std::make_unique<Camera>(m_Width, m_Height, m_Renderer);

		// Collider Debugger

		m_ColliderDebugger = std::make_unique<ColliderDebugRenderer>(m_Renderer);

		OnUserCreate();
	}

	void Engine2D::HandleEvents() {
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
				SDL_SetWindowSize(m_Window, m_Width, m_Height);
				SDL_SetWindowFullscreen(m_Window, 0);
				m_Level->RenderToBuffer();
				break;
			}
		}
		m_Player->HandleEvents(m_Event);
	}

	void Engine2D::Update() {
		m_Player->Update(m_DeltaTime);
		m_PlayerCollider->Update(m_DeltaTime);
		OnUserUpdate();
	}

	void Engine2D::Render() {
		m_Camera->ClearCameraBuffer();

		m_Level->Render(m_Camera);
		m_Player->Render(m_Camera);

		OnUserRender();

		m_Camera->Render();

		SDL_RenderPresent(m_Renderer);

		if (!InFocus()) {
			SDL_WaitEvent(NULL); //If Window not in focus, do not render and wait for window to come in focus.
		}
	}
}