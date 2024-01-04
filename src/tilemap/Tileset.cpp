#include "Tileset.hpp"
#include "../TextureLoader.hpp"

Tileset::Tileset(TilesetConfig& config, const char* atlasPath, SDL_Renderer* renderer)
	: m_Config(std::move(config))
{
	m_Atlas = TextureUtil::LoadTexture(atlasPath, renderer);
	m_Columns = (m_Config.imageWidth + m_Config.spacing - (2 * m_Config.margin)) / (m_Config.tileSize + m_Config.spacing);
}

Tileset::~Tileset() {
	SDL_DestroyTexture(m_Atlas);
}

void Tileset::GetTile(const int tileId, SDL_Rect& rect) {
	unsigned int column = (tileId - 1) % m_Columns;
	unsigned int row = (tileId - 1) / m_Columns;

	rect.x = (column * (m_Config.tileSize + m_Config.spacing)) + m_Config.spacing / 2;
	rect.y = (row * (m_Config.tileSize + m_Config.spacing)) + m_Config.spacing / 2;
}