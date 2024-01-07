#pragma once
#include "SDL.h"
#include "json.hpp"

namespace notval {

	static constexpr unsigned int FlippedHorizontallyFlag = 0x80000000;
	static constexpr unsigned int FlippedVerticallyFlag = 0x40000000;
	static constexpr unsigned int FlippedAntiDiagonallyFlag = 0x20000000;

	struct TilesetConfig {
		unsigned int tileSize;
		unsigned int firstGID;
		unsigned int imageWidth, imageHeight;
		unsigned int margin;
		unsigned int spacing;
	};

	class Tileset {
	public:
		Tileset(TilesetConfig& config, const char* atlasPath, SDL_Renderer* renderer);
		~Tileset();

		Tileset(const Tileset& other) = delete;
		Tileset(const Tileset&& other) = delete;

		Tileset& operator=(const Tileset& other) = delete;
		Tileset& operator=(const Tileset&& other) = delete;

		bool GetTile(const unsigned int tileId, SDL_Rect& rect, double& angle, SDL_RendererFlip& flipFlag) const;
		inline SDL_Texture* GetAtlas() const { return m_Atlas; }
	private:
		TilesetConfig m_Config;
		unsigned int m_Columns;
		SDL_Texture* m_Atlas;

		void GetFlags(const unsigned int tileId, double& angle, SDL_RendererFlip& flipFlag) const;
	};

}