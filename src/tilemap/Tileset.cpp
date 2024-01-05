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

#include <iostream>

bool Tileset::GetTile(const unsigned int tileId, SDL_Rect& rect, double& angle, SDL_RendererFlip& flipFlag) const {
	GetFlags(tileId, angle, flipFlag);

	unsigned int localID = (tileId & 0x0FFFFFFF) - m_Config.firstGID;

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

void Tileset::GetFlags(const unsigned int tileId, double& angle, SDL_RendererFlip& flipFlag) const {
	// FlippedVerticallyFlag & FlippedHorizontallyFlag & FlippedAntiDiagonallyFlag is same as 0xE0000000
	unsigned int setFlags = tileId & 0xE0000000;

	switch (setFlags) {
		case(0x00000000):
			break;
		case(FlippedHorizontallyFlag):
			flipFlag = SDL_FLIP_HORIZONTAL;
			break;
		case(FlippedVerticallyFlag):
			flipFlag = SDL_FLIP_VERTICAL;
			break;
		case(FlippedAntiDiagonallyFlag):
			flipFlag = SDL_FLIP_VERTICAL;
			angle = 90;
			break;
		case(FlippedHorizontallyFlag | FlippedVerticallyFlag):
			angle = 180;
			break;
		case(FlippedHorizontallyFlag | FlippedAntiDiagonallyFlag):
			break;
		case(FlippedVerticallyFlag | FlippedAntiDiagonallyFlag):

			break;
		case(FlippedVerticallyFlag | FlippedHorizontallyFlag | FlippedAntiDiagonallyFlag):
			break;
		default:
			break;
	}
}