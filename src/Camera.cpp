#include "Camera.hpp"

Camera::Camera(const int width, const int height, SDL_Renderer* renderer) : m_CameraRect({ 0, 0, width, height }), m_Renderer(renderer) {
	m_Buffer = SDL_CreateTexture(m_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
}

Camera::~Camera() {
	SDL_DestroyTexture(m_Buffer);
}

void Camera::RenderToBuffer(SDL_Texture* incomingBuffer, const SDL_Rect* srcRect, const SDL_Rect* destRect) const {
	SDL_Texture* renderTarget = SDL_GetRenderTarget(m_Renderer);

	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	SDL_RenderCopy(m_Renderer, incomingBuffer, srcRect, destRect);

	SDL_SetRenderTarget(m_Renderer, renderTarget);
}

void Camera::Render() const {
	SDL_Texture* renderTarget = SDL_GetRenderTarget(m_Renderer);

	SDL_SetRenderTarget(m_Renderer, NULL);
	SDL_RenderClear(m_Renderer);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	SDL_RenderCopy(m_Renderer, m_Buffer, &m_CameraRect, NULL);

	SDL_SetRenderTarget(m_Renderer, renderTarget);
}

void Camera::SetDimensions(const int width, const int height) {
	m_CameraRect.w = width;
	m_CameraRect.h = height;
}