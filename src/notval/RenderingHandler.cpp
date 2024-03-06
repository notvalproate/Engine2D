#include "Core.hpp"

RenderingHandler::RenderingHandler() : m_Renderer(nullptr), m_AvailableSortingLayers({ "Default" }), m_CirclePoints(1000) { }

bool RenderingHandler::InitRenderer() {
	if (!(m_Renderer = SDL_CreateRenderer(Object::Screen.m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE))) {
		std::cout << "Error: Couldn't Initialize Renderer..." << std::endl;
		return false;
	}

	SDL_SetRenderTarget(m_Renderer, NULL);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

	std::cout << "Stage: Initialized Renderer..." << std::endl;

	SDL_RendererInfo info;
	SDL_GetRendererInfo(m_Renderer, &info);
	std::cout << "Using Renderer:" << info.name << std::endl;

	return true;
}

void RenderingHandler::SetRendererVsync(const bool set) {
	SDL_RenderSetVSync(m_Renderer, set ? 1 : 0);
}

void RenderingHandler::RenderPoint(const Vector2D& point, const uint8_t width, const Color color) const {
	Camera* currentCamera = Object::SceneManager.GetCurrentCamera();

	if (!currentCamera) {
		return;
	}

	double pixelsPerUnitOnScreen = currentCamera->GetPixelsPerUnit();

	Vector2D screenPosition = currentCamera->WorldToScreenPoint(point);

	int initialX = screenPosition.x - (width / 2);
	int maxX = screenPosition.x + ((width - 1) / 2);
	int initialY = screenPosition.y - (width / 2);
	int maxY = screenPosition.y + ((width - 1) / 2);

	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	for (int x = initialX; x <= maxX; x++) {
		for (int y = initialY; y <= maxY; y++) {
			SDL_RenderDrawPoint(m_Renderer, x, y);
		}
	}
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
}

void RenderingHandler::RenderLine(const Vector2D& src, const Vector2D& dest, const Color color) const {
	Camera* currentCamera = Object::SceneManager.GetCurrentCamera();

	if (!currentCamera) {
		return;
	}

	double pixelsPerUnitOnScreen = currentCamera->GetPixelsPerUnit();

	Vector2D srcScreenPosition = currentCamera->WorldToScreenPoint(src);
	Vector2D destScreenPosition = currentCamera->WorldToScreenPoint(dest);

	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(m_Renderer, srcScreenPosition.x, srcScreenPosition.y, destScreenPosition.x, destScreenPosition.y);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
}

void RenderingHandler::RenderRect(const Vector2D& position, const Vector2D& dimensions, const Color color) const {
	Camera* currentCamera = Object::SceneManager.GetCurrentCamera();

	if (!currentCamera) {
		return;
	}

	double pixelsPerUnitOnScreen = currentCamera->GetPixelsPerUnit();

	Vector2D screenPosition = currentCamera->WorldToScreenPoint(position);
	Vector2D newDimensions = dimensions * pixelsPerUnitOnScreen;

	SDL_Rect rect{
		screenPosition.x - newDimensions.x / 2.0,
		screenPosition.y - newDimensions.y / 2.0,
		newDimensions.x,
		newDimensions.y,
	};

	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(m_Renderer, &rect);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
}

int RenderingHandler::RoundUpToMultipleOfEight(const int v) const {
	return (v + (8 - 1)) & -8;
}

void RenderingHandler::RenderCircle(const Vector2D& center, const double radius, const Color color) const {
	Camera* currentCamera = Object::SceneManager.GetCurrentCamera();

	if (!currentCamera) {
		return;
	}

	SDL_SetRenderDrawColor(m_Renderer, color.r, color.g, color.b, color.a);

	// Circle Rendering Algorithm using Midpoint Circle Algorithm
	// Scotty Stephens & JanSordid: https://stackoverflow.com/a/48291620 & https://stackoverflow.com/a/74745126
	const int vecSize = RoundUpToMultipleOfEight(radius * 8 * 35 / 49);

	if (vecSize > m_CirclePoints.size()) {
		m_CirclePoints.resize(vecSize);
	}

	Vector2D screenCenter = currentCamera->WorldToScreenPoint(center);
	double screenRadius = radius * currentCamera->GetPixelsPerUnit();

	int drawCount = 0;

	const int diameter = screenRadius * 2;
	const int centerX = screenCenter.x;
	const int centerY = screenCenter.y;

	int x = screenRadius - 1;
	int y = 0;
	int tx = 1;
	int ty = 1;
	int error = tx - diameter;

	while (x >= y) {
		m_CirclePoints.at(drawCount++) = { centerX + x, centerY - y };
		m_CirclePoints.at(drawCount++) = { centerX + x, centerY + y };
		m_CirclePoints.at(drawCount++) = { centerX - x, centerY - y };
		m_CirclePoints.at(drawCount++) = { centerX - x, centerY + y };
		m_CirclePoints.at(drawCount++) = { centerX + y, centerY - x };
		m_CirclePoints.at(drawCount++) = { centerX + y, centerY + x };
		m_CirclePoints.at(drawCount++) = { centerX - y, centerY - x };
		m_CirclePoints.at(drawCount++) = { centerX - y, centerY + x };

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}

	SDL_RenderDrawPoints(m_Renderer, &m_CirclePoints[0], drawCount);

	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
}

void RenderingHandler::RenderSprite(SDL_Texture* texture, const Vector2D& dimensions, const uint16_t pixelsPerUnit, const Transform* transform) const {
	Camera* currentCamera = Object::SceneManager.GetCurrentCamera();

	if (!currentCamera) {
		return;
	}

	double angle = transform->rotation;
	SDL_RendererFlip flipFlag;

	SDL_FRect destRect = GetSpriteDestRect(dimensions, pixelsPerUnit, transform, currentCamera);
	GetFlipAndRotation(transform, angle, flipFlag);
	
	SDL_RenderCopyExF(m_Renderer, texture, NULL, &destRect, angle, NULL, flipFlag);
}

SDL_FRect RenderingHandler::GetSpriteDestRect(const Vector2D& dimensions, const uint16_t pixelsPerUnit, const Transform* transform, const Camera* currentCamera) const {
	double pixelsPerUnitOnScreen = currentCamera->GetPixelsPerUnit();

	Vector2D screenPosition = currentCamera->WorldToScreenPoint(transform->position);
	Vector2D newDimensions = (dimensions * pixelsPerUnitOnScreen) / (double)pixelsPerUnit;

	newDimensions.x = newDimensions.x * std::abs(transform->scale.x);
	newDimensions.y = newDimensions.y * std::abs(transform->scale.y);

	SDL_FRect destRect{
		(2 * screenPosition.x - newDimensions.x) / 2.0,
		(2 * screenPosition.y - newDimensions.y) / 2.0,
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
}

void RenderingHandler::DestroyRenderer() {
	SDL_DestroyRenderer(m_Renderer);
}