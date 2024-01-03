#pragma once
#include "SDL.h"

class Camera {
public:
	Camera(const int width, const int height, SDL_Renderer* renderer);
	~Camera();

	Camera(const Camera& other) = delete;
	Camera(const Camera&& other) = delete;

	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(const Camera&& other) = delete;

	void Render() const;
	void RenderToBuffer(SDL_Texture* buffer, const SDL_Rect* bufferRect) const;
private:
	SDL_Texture* m_Buffer;
	SDL_Renderer* m_Renderer;
	SDL_Rect m_CameraRect;
};