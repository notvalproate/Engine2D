#include "Core.hpp"
#include "SDL_image.h"

Engine2D::Engine2D() : m_CurrentScene(nullptr), m_IsRunning(true), m_Renderer(nullptr), m_DeltaTime(16), m_Window(nullptr) { }

void Engine2D::InitGame(const char* title, const char* iconpath, int windowWidth, int windowHeight) {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		std::cout << "Error: Couldn't Initialize Subsystems..." << std::endl;
		return;
	}
	std::cout << "Stage: Initialized Subsystems..." << std::endl;

	if (SDL_GetDisplayMode(0, 0, &m_Mode)) {
		std::cout << "Error: Couldn't Get Display Mode...Framerate set to 60" << std::endl;
	}
	else {
		std::cout << "Stage: Display Mode Initialized..." << std::endl;
		m_DeltaTime = (float)1.0 / (float)m_Mode.refresh_rate;
	}

	m_Width = windowWidth;
	m_Height = windowHeight;

	if (!(m_Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0))) {
		std::cout << "Error: Couldn't Initialize Window..." << std::endl;
		return;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	SDL_Surface* TempSurface = IMG_Load(iconpath);
	SDL_SetWindowIcon(m_Window, TempSurface); //Setting window icon
	SDL_FreeSurface(TempSurface);
	std::cout << "Stage: Initialized Window..." << std::endl;

	if (!(m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE))) {
		std::cout << "Error: Couldn't Initialize Renderer..." << std::endl;
		return;
	}

	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	std::cout << "Stage: Initialized Renderer..." << std::endl;

	SetupGame();
}

Engine2D::~Engine2D() {
	SDL_DestroyWindow(m_Window);
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

	m_CurrentScene->Update();
}

void Engine2D::Render() const {
	m_CurrentScene->Render();

	SDL_RenderPresent(m_Renderer);

	if (!InFocus()) {
		SDL_WaitEvent(NULL);
	}
}

void Engine2D::LoadScene(std::size_t sceneID) {
	m_CurrentScene = m_Scenes[sceneID].get();
	m_CurrentScene->SetupScene();
}