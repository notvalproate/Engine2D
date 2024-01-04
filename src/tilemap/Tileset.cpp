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

bool Tileset::GetTile(const unsigned int tileId, SDL_Rect& rect) const {
	unsigned int localID = tileId - m_Config.firstGID;

	unsigned int column = localID % m_Columns;
	unsigned int row = localID / m_Columns;

	unsigned int x = m_Config.margin + ((m_Config.tileSize + m_Config.spacing) * column);
	unsigned int y = m_Config.margin + ((m_Config.tileSize + m_Config.spacing) * row);

	if (y > m_Config.imageHeight) {
		return false;
	}

	rect.x = x;
	rect.y = y;

	return true;
}