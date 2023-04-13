#include "Tilemap.hpp"
#include "TextureLoader.hpp"

Tilemap::Tilemap(unsigned short* p_FgTileMap, unsigned short* p_BgTileMap, const char* p_BgPath, const char* p_TilesPath, const std::vector<SDL_Rect>& p_Tiles, SDL_Renderer* p_Renderer) {
	SDL_Texture* TilemapTex = TextureUtil::LoadTexture(p_TilesPath, p_Renderer); 
	SDL_Texture* BgTex = TextureUtil::LoadTexture(p_BgPath, p_Renderer);

	m_Renderer = p_Renderer;
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 320, 180);

	SDL_Rect Temp = { 0, 0, 8, 8 };

	SDL_SetRenderTarget(p_Renderer, m_Buffer);

	SDL_RenderCopy(p_Renderer, BgTex, NULL, NULL);
	int k;

	for (int i = 0; i < 23; i++) {
		Temp.y = i * 8;
		for (int j = 0; j < 40; j++) {
			Temp.x = j * 8;
			k = (i * 40) + j;

			if (p_FgTileMap[k] != 0) {
				SDL_RenderCopy(p_Renderer, TilemapTex, &p_Tiles[p_FgTileMap[k] - 1], &Temp);
			}
			if (p_BgTileMap[k] != 0) {
				SDL_RenderCopy(p_Renderer, TilemapTex, &p_Tiles[p_BgTileMap[k] - 1], &Temp);
			}
		}
	}

	SDL_SetRenderTarget(p_Renderer, NULL);
}

Tilemap::~Tilemap() { 
	SDL_DestroyTexture(m_Buffer);
}

void Tilemap::Render(){
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, NULL);
}