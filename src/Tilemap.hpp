#pragma once
#include "SDL.h"
#include <vector>

class Tilemap {
public:
	Tilemap(unsigned short* p_TileMap, const char* p_TilesPath, const std::vector<SDL_Rect>& p_Tiles, SDL_Renderer* p_Renderer);
	~Tilemap();

	void Render();
private:
	unsigned short* m_Map;
	std::vector<SDL_Rect> m_Tiles;
	SDL_Texture *m_TilemapTex, *m_Buffer;
	SDL_Renderer* m_Renderer;
};