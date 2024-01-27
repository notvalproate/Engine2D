#include "Core.hpp"
#include "SDL_image.h"

Engine2D::Engine2D() : m_IsRunning(true), m_Renderer(nullptr), m_DeltaTime(16) { }

void Engine2D::InitGame(const char* title, const char* iconpath, int windowWidth, int windowHeight) {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		std::cout << "Error: Couldn't Initialize Subsystems..." << std::endl;
		return;
	}
	std::cout << "Stage: Initialized Subsystems..." << std::endl;

	Screen.InitScreen(title, iconpath, windowWidth, windowHeight);

	m_DeltaTime = (float)1.0 / (float)Screen.m_Mode.refresh_rate;

	SDL_Surface* TempSurface = IMG_Load(iconpath);
	SDL_SetWindowIcon(Screen.m_Window, TempSurface); //Setting window icon
	SDL_FreeSurface(TempSurface);
	std::cout << "Stage: Initialized Window..." << std::endl;

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	if (!(m_Renderer = SDL_CreateRenderer(Screen.m_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE))) {
		std::cout << "Error: Couldn't Initialize Renderer..." << std::endl;
		return;
	}

	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	std::cout << "Stage: Initialized Renderer..." << std::endl;

	SetupGame();
}

Engine2D::~Engine2D() {
	SDL_DestroyWindow(Screen.m_Window);
	SDL_DestroyRenderer(m_Renderer);
	SDL_Quit();
}

void Engine2D::Run() {
	while (m_IsRunning) {
		Update();
		Render();
	}
}

void Engine2D::Update() {
	Input.PollEvents();

	if (Input.GetCurrentEvent().type == SDL_QUIT) {
		m_IsRunning = false;
	}

	SceneManager.m_CurrentScene->Update();
}

void Engine2D::Render() const {
	SceneManager.m_CurrentScene->Render();

	SDL_RenderPresent(m_Renderer);

	if (!Screen.InFocus()) {
		SDL_WaitEvent(NULL);
	}
}