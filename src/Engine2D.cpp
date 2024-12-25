#include <iostream>

#include "SDL_image.h"

#include "Engine2D.hpp"
#include "TimeHandler.hpp"
#include "ScreenHandler.hpp"
#include "SceneHandler.hpp"
#include "CursorHandler.hpp"

namespace engine2d {

Engine2D::Engine2D() : m_IsRunning(true) { }

void Engine2D::InitGame(const char* title, const std::filesystem::path& iconpath, int windowWidth, int windowHeight) {
	if (SDL_Init(
		SDL_INIT_VIDEO | 
		SDL_INIT_EVENTS |
		SDL_INIT_AUDIO
		) != 0) {
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
	Input.InitInput();
	Cursor.InitSystemCursors();

	SetupGame();
}

Engine2D::~Engine2D() {
	SceneManager.DestroyScenes();
	Screen.DestroyScreen();
	RenderingPipeline.DestroyRenderer();
	Cursor.DestroySystemCursors();
	SDL_Quit();
}

void Engine2D::Run() {
	while (m_IsRunning) {
		Time.UpdateDeltaTime();
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

} // namespace engine2d