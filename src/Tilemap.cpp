#include "Tilemap.hpp"
#include "TextureLoader.hpp"

Tilemap::Tilemap(unsigned short* p_TileMap, const char* p_TilesPath, const std::vector<SDL_Rect>& p_Tiles, SDL_Renderer* p_Renderer) {
	m_Map = p_TileMap;
	m_Tiles = p_Tiles;
	m_Renderer = p_Renderer;
	m_TilemapTex = TextureUtil::LoadTexture(p_TilesPath, p_Renderer);
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 320, 180);

	SDL_Rect Temp = { 0, 0, 8, 8 };

	SDL_SetRenderTarget(p_Renderer, m_Buffer);
	for (int i = 0; i < 23; i++) {
		Temp.y = i * 8;
		for (int j = 0; j < 40; j++) {
			Temp.x = j * 8;
			if (p_TileMap[(i * 40) + j] == 0) {
				continue;
			}
			SDL_RenderCopy(p_Renderer, m_TilemapTex, &m_Tiles[p_TileMap[(i * 40) + j] - 1], &Temp);
		}
	}
	SDL_SetRenderTarget(p_Renderer, NULL);
}

Tilemap::~Tilemap() { }

void Tilemap::Render(){
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
}