#include "Tilemap.hpp"
#include "../TextureLoader.hpp"

Tilemap::Tilemap(const unsigned short p_TileSize, const char* p_TilesPath, const std::vector<SDL_Rect>& p_Tiles, SDL_Renderer* p_Renderer, const int& p_Width, const int& p_Height) {
	m_TilemapTex = TextureUtil::LoadTexture(p_TilesPath, p_Renderer);
	m_TileSize = p_TileSize;
	m_Tiles = p_Tiles;
	m_Renderer = p_Renderer;
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, p_TileSize * p_Width, p_TileSize * p_Height);
	m_Collider = nullptr;
	m_Width = p_Width;
	m_Height = p_Height;
	m_Camera = { 0, 0, 320, 180 };
}

Tilemap::~Tilemap() { 
	SDL_DestroyTexture(m_Buffer);
	delete[] m_Collider;
}

void Tilemap::AddLayer(unsigned short* p_TileMap) {
	m_TileLayers.push_back(p_TileMap);
}

void Tilemap::SetBackground(const std::string& p_TexPath) {
	m_Background = p_TexPath;
}

void Tilemap::AddBackgroundProps(const std::string& p_TexPath) {
	m_BackgroundProps = p_TexPath;
}

void Tilemap::AddForegroundProps(const std::string& p_TexPath) {
	m_ForegroundProps = p_TexPath;
}

void Tilemap::SetCollider(unsigned short* p_Collider) {
	m_Collider = p_Collider;
}

void Tilemap::RenderToBuffer() {

	SDL_SetRenderTarget(m_Renderer, m_Buffer);

	SDL_RenderCopy(m_Renderer, TextureUtil::LoadTexture(m_Background.c_str(), m_Renderer), NULL, NULL);

	SDL_RenderCopy(m_Renderer, TextureUtil::LoadTexture(m_BackgroundProps.c_str(), m_Renderer), NULL, NULL);

	for (int n = 0; n < m_TileLayers.size(); n++) {
		RenderTiles(n);
	}

	SDL_RenderCopy(m_Renderer, TextureUtil::LoadTexture(m_ForegroundProps.c_str(), m_Renderer), NULL, NULL);

	SDL_SetRenderTarget(m_Renderer, NULL);
}

void Tilemap::RenderTiles(const int& p_n) {
	int k;
	SDL_Rect Temp = { 0, 0, m_TileSize, m_TileSize };

	for (int i = 0; i < m_Height; i++) {
		Temp.y = i * m_TileSize;
		for (int j = 0; j < m_Width; j++) {
			Temp.x = j * m_TileSize;
			k = (i * m_Width) + j;

			if (m_TileLayers[p_n][k] != 0) {
				SDL_RenderCopy(m_Renderer, m_TilemapTex, &m_Tiles[m_TileLayers[p_n][k] - 1], &Temp);
			}
		}
	}
}

void Tilemap::Render() const {
	SDL_RenderCopy(m_Renderer, m_Buffer, &m_Camera, NULL);
}

void Tilemap::SaveTilemapAsPng(const char* file_name) const {
	SDL_Texture* target = SDL_GetRenderTarget(m_Renderer);
	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	int width, height;
	SDL_QueryTexture(m_Buffer, NULL, NULL, &width, &height);
	SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_RenderReadPixels(m_Renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
	IMG_SavePNG(surface, file_name);
	SDL_FreeSurface(surface);
	SDL_SetRenderTarget(m_Renderer, target);
}