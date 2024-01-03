#pragma once
#include "SDL.h"
#include "json.hpp"

class Tileset {
public:
	Tileset(const unsigned int tileSize, const unsigned int firstGid, const unsigned int imgWidth, const unsigned int imgHeight, const unsigned int spacing, const char* atlasPath, SDL_Renderer* renderer);
	~Tileset();

	Tileset(const Tileset& other) = delete;
	Tileset(const Tileset&& other) = delete;

	Tileset& operator=(const Tileset& other) = delete;
	Tileset& operator=(const Tileset&& other) = delete;

	void GetTile(const int tileId, SDL_Rect& rect);
private:
	const unsigned int m_TileSize;
	const unsigned int m_FirstGID;
	const unsigned int m_ImageWidth, m_ImageHeight;
	const unsigned int m_Spacing;
	const unsigned int m_Columns, m_Rows;
	SDL_Texture* m_Atlas;
};