#pragma once
#include "SDL.h"

namespace notval {

	class Camera {
	public:
		Camera(const int width, const int height, SDL_Renderer* renderer);
		~Camera();

		Camera(const Camera& other) = delete;
		Camera(const Camera&& other) = delete;

		Camera& operator=(const Camera& other) = delete;
		Camera& operator=(const Camera&& other) = delete;

		void Render() const;
		void RenderToBuffer(SDL_Texture* buffer, const SDL_Rect* srcRect, const SDL_Rect* destRect) const;
		void ClearCameraBuffer() const;

		void SetDimensions(const int width, const int height);
		void SetPosition(const int x, const int y);
	private:
		SDL_Texture* m_BackgroundBuffer;
		SDL_Renderer* m_Renderer;
		SDL_Rect m_CameraRect;
	};

}