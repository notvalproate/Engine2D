#include "Core.hpp"
#include "SDL_image.h"

ScreenHandler::ScreenHandler() : m_Window(nullptr) { }

void ScreenHandler::ToggleFullscreen() {
	if (!IsFullscreen()) {
		SetToDisplayResolution();
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN);
		return;
	}

	SDL_SetWindowFullscreen(m_Window, 0);
	SetToInitialResolution();
}

void ScreenHandler::SetResolution(const int w, const int h) {
	m_Width = w;
	m_Height = h;
	m_AspectRatio = static_cast<double>(m_Width) / static_cast<double>(m_Height);
	SDL_SetWindowSize(m_Window, w, h);
	SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void ScreenHandler::SetToInitialResolution() {
	SetResolution(m_InitWidth, m_InitHeight);
}

void ScreenHandler::SetToDisplayResolution() {
	SetResolution(m_Mode.w, m_Mode.h);
}

bool ScreenHandler::InitScreen(const std::string& title, const std::filesystem::path iconpath, const int windowWidth, const int windowHeight) {
	if (SDL_GetDisplayMode(0, 0, &m_Mode)) {
		std::cout << "Error: Couldn't Get Display Mode...Framerate set to 60" << std::endl;
	}
	else {
		std::cout << "Stage: Display Mode Initialized..." << std::endl;
	}

	m_InitWidth = windowWidth;
	m_InitHeight = windowHeight;
	m_Width = windowWidth;
	m_Height = windowHeight;
	m_AspectRatio = static_cast<double>(m_Width) / static_cast<double>(m_Height);

	if (!(m_Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0))) {
		std::cout << "Error: Couldn't Initialize Window..." << std::endl;
		return false;
	}

	std::cout << "Stage: Initialized Window..." << std::endl;

	if (std::filesystem::exists(iconpath)) {
		SDL_Surface* TempSurface = IMG_Load(iconpath.string().c_str());
		SDL_SetWindowIcon(m_Window, TempSurface);
		SDL_FreeSurface(TempSurface);
	} 
	else {
		std::cerr << "File path to icon provided is invalid! Path: " << iconpath << std::endl;
	}

	std::cout << "Stage: Initialized Window..." << std::endl;

	SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);

	return true;
}

void ScreenHandler::DestroyScreen() {
	SDL_DestroyWindow(m_Window);
}