#include "Core.hpp"

ScreenHandler::ScreenHandler() : m_Window(nullptr) { }

void ScreenHandler::ToggleFullscreen() {
	if (!(SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN)) {
		SDL_SetWindowSize(m_Window, m_Mode.w, m_Mode.h); 
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN);
		return;
	}

	SDL_SetWindowSize(m_Window, m_Width, m_Height);
	SDL_SetWindowFullscreen(m_Window, 0);
}

void ScreenHandler::InitScreen(const char* title, const char* iconpath, const int windowWidth, const int windowHeight) {
	if (SDL_GetDisplayMode(0, 0, &m_Mode)) {
		std::cout << "Error: Couldn't Get Display Mode...Framerate set to 60" << std::endl;
	}
	else {
		std::cout << "Stage: Display Mode Initialized..." << std::endl;
	}

	m_Width = windowWidth;
	m_Height = windowHeight;

	if (!(m_Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0))) {
		std::cout << "Error: Couldn't Initialize Window..." << std::endl;
		return;
	}

	std::cout << "Stage: Initialized Window..." << std::endl;
}