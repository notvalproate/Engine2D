#include "Tileset.hpp"
#include "../TextureLoader.hpp"

Tileset::Tileset(const unsigned int tileSize, const unsigned int firstGid, const unsigned int imgWidth, const unsigned int imgHeight, const unsigned int spacing, const char* atlasPath, SDL_Renderer* renderer)
	: 
	m_TileSize(tileSize), 
	m_FirstGID(firstGid), 
	m_ImageWidth(imgWidth),
	m_ImageHeight(imgHeight), 
	m_Spacing(spacing), 
	m_Columns(imgWidth / (m_TileSize + m_Spacing)), 
	m_Rows(imgHeight / (m_TileSize + m_Spacing))
{
	m_Atlas = TextureUtil::LoadTexture(atlasPath, renderer);
}

Tileset::~Tileset() {
	SDL_DestroyTexture(m_Atlas);
}

void Tileset::GetTile(const int tileId, SDL_Rect& rect) {
	unsigned int column = (tileId - 1) % m_Columns;
	unsigned int row = (tileId - 1) / m_Columns;

	rect.w = m_TileSize;
	rect.h = m_TileSize;

	rect.x = (column * (m_TileSize + m_Spacing)) + m_Spacing / 2;
	rect.y = (row * (m_TileSize + m_Spacing)) + m_Spacing / 2;
}