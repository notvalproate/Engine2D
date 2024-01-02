#pragma once
#include "SDL.h"

class Tileset {
public:
	Tileset();
	~Tileset();

	Tileset(const Tileset& other) = delete;
	Tileset(const Tileset&& other) = delete;

	Tileset& operator=(const Tileset& other) = delete;
	Tileset& operator=(const Tileset&& other) = delete;
private:
	SDL_Texture* m_Atlas;
};