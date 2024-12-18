#include "Camera.hpp"

namespace notval {

	Camera::Camera(const int width, const int height, SDL_Renderer* renderer) : m_CameraRect({ 0, 0, width, height }), m_Renderer(renderer) {
		m_BackgroundBuffer = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
		SDL_SetTextureBlendMode(m_BackgroundBuffer, SDL_BLENDMODE_BLEND);
	}

	Camera::~Camera() {
		SDL_DestroyTexture(m_BackgroundBuffer);
	}

	void Camera::ClearCameraBuffer() const {
		SDL_Texture* renderTarget = SDL_GetRenderTarget(m_Renderer);

		SDL_SetRenderTarget(m_Renderer, m_BackgroundBuffer);
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		SDL_RenderClear(m_Renderer);

		SDL_SetRenderTarget(m_Renderer, renderTarget);
	}

	void Camera::RenderToBuffer(SDL_Texture* incomingBuffer, const SDL_Rect* srcRect, const SDL_Rect* destRect) const {
		SDL_Texture* renderTarget = SDL_GetRenderTarget(m_Renderer);

		SDL_Rect newDest = {
			destRect->x - m_CameraRect.x,
			destRect->y - m_CameraRect.y,
			destRect->w,
			destRect->h
		};

		SDL_SetRenderTarget(m_Renderer, m_BackgroundBuffer);
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);

		SDL_RenderCopy(m_Renderer, incomingBuffer, srcRect, &newDest);

		SDL_SetRenderTarget(m_Renderer, renderTarget);
	}

	void Camera::Render() const {
		SDL_Texture* renderTarget = SDL_GetRenderTarget(m_Renderer);

		SDL_SetRenderTarget(m_Renderer, NULL);
		SDL_RenderClear(m_Renderer);
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		SDL_RenderCopy(m_Renderer, m_BackgroundBuffer, NULL, NULL);

		SDL_SetRenderTarget(m_Renderer, renderTarget);
	}

	void Camera::SetDimensions(const int width, const int height) {
		m_CameraRect.w = width;
		m_CameraRect.h = height;

		SDL_DestroyTexture(m_BackgroundBuffer);
		m_BackgroundBuffer = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
		SDL_SetTextureBlendMode(m_BackgroundBuffer, SDL_BLENDMODE_BLEND);
	}

	void Camera::SetPosition(const int x, const int y) {
		m_CameraRect.x = x;
		m_CameraRect.y = y;
	}

}