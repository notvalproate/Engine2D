#include "Core.hpp"
#include "SDL_image.h"

Engine2D::Engine2D() : m_IsRunning(true) { }

void Engine2D::InitGame(const char* title, const char* iconpath, int windowWidth, int windowHeight) {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		std::cout << "Error: Couldn't Initialize Subsystems..." << std::endl;
		m_IsRunning = false;
		return;
	}
	std::cout << "Stage: Initialized Subsystems..." << std::endl;

	if (!Screen.InitScreen(title, iconpath, windowWidth, windowHeight)) {
		m_IsRunning = false;
	}

	if (!RenderingPipeline.InitRenderer()) {
		m_IsRunning = false;
	}

	Time.InitTime();

	SetupGame();
}

Engine2D::~Engine2D() {
	SDL_DestroyWindow(Screen.m_Window);
	SDL_DestroyRenderer(RenderingPipeline.m_Renderer);
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
	RenderingPipeline.PresentRenderer();
}