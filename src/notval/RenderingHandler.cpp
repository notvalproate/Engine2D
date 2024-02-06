#include "Core.hpp"

RenderingHandler::RenderingHandler() : m_Renderer(nullptr), m_AvailableSortingLayers({ "Default" }) { }

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

void RenderingHandler::RenderSprite(SDL_Texture* texture, const Vector2D dimensions, const uint16_t pixelsPerUnit, const Transform* transform) {
	double angle = transform->rotation;
	SDL_RendererFlip flipFlag;

	SDL_Rect destRect = GetSpriteDestRect(dimensions, pixelsPerUnit, transform);
	GetFlipAndRotation(transform, angle, flipFlag);
	
	SDL_RenderCopyEx(m_Renderer, texture, NULL, &destRect, angle, NULL, flipFlag);
}

SDL_Rect RenderingHandler::GetSpriteDestRect(const Vector2D dimensions, const uint16_t pixelsPerUnit, const Transform* transform) const {
	Camera* currentCamera = Object::SceneManager.GetCurrentCamera();
	double pixelsPerUnitOnScreen = currentCamera->GetPixelsPerUnit();

	Vector2D screenPosition = currentCamera->WorldToScreenPoint(transform->position);
	Vector2D newDimensions = (dimensions * pixelsPerUnitOnScreen) / pixelsPerUnit;

	newDimensions.x = newDimensions.x * std::abs(transform->scale.x);
	newDimensions.y = newDimensions.y * std::abs(transform->scale.y);

	SDL_Rect destRect{
		screenPosition.x - newDimensions.x / 2.0,
		screenPosition.y - newDimensions.y / 2.0,
		newDimensions.x,
		newDimensions.y,
	};

	return destRect;
}

void RenderingHandler::GetFlipAndRotation(const Transform* transform, double& rotation, SDL_RendererFlip& flipFlag) const {
	if (transform->scale.x < 0 && transform->scale.y < 0) {
		rotation += 180.0;
	}
	else if (transform->scale.x < 0) {
		flipFlag = SDL_FLIP_HORIZONTAL;
	}
	else if (transform->scale.y < 0) {
		flipFlag = SDL_FLIP_VERTICAL;
	}
	else {
		flipFlag = SDL_FLIP_NONE;
	}
}

void RenderingHandler::AddSortingLayer(const std::string& name) {
	m_AvailableSortingLayers.push_back(name);
}

const std::vector<std::string>& RenderingHandler::GetAvailableSortingLayers() const {
	return m_AvailableSortingLayers;
}

void RenderingHandler::PresentRenderer() {
	SDL_RenderPresent(m_Renderer);
	SDL_RenderClear(m_Renderer);

	if (!Object::Screen.InFocus()) {
		SDL_WaitEvent(NULL);
	}
}