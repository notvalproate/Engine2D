#include "Core.hpp"

RenderingHandler::RenderingHandler() : m_Renderer(nullptr) { }

bool RenderingHandler::InitRenderer() {
	if (!(m_Renderer = SDL_CreateRenderer(Object::Screen.m_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE))) {
		std::cout << "Error: Couldn't Initialize Renderer..." << std::endl;
		return false;
	}

	SDL_SetRenderTarget(m_Renderer, NULL);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	std::cout << "Stage: Initialized Renderer..." << std::endl;

	return true;
}

void RenderingHandler::RenderSprite(SDL_Texture* texture, const SDL_Rect src, const SDL_Rect dest, const double angle) {
	SDL_RenderCopyEx(m_Renderer, texture, &src, &dest, angle, NULL, SDL_FLIP_NONE);
}

void RenderingHandler::PresentRenderer() {
	SDL_RenderPresent(m_Renderer);
	SDL_RenderClear(m_Renderer);

	if (!Object::Screen.InFocus()) {
		SDL_WaitEvent(NULL);
	}
}