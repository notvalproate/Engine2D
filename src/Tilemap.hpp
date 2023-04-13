#pragma once
#include "SDL.h"
#include <vector>

class Tilemap {
public:
	Tilemap(unsigned short* p_FgTileMap, unsigned short* p_BgTileMap, const char* p_BgPath, const char* p_TilesPath, const std::vector<SDL_Rect>& p_Tiles, SDL_Renderer* p_Renderer);
	~Tilemap();

	void Render();
private:
	SDL_Texture *m_Buffer;
	SDL_Renderer* m_Renderer;
};