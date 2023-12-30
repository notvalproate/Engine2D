#include "Tilemap.hpp"
#include "../TextureLoader.hpp"

Tilemap::Tilemap(const unsigned short p_TileSize, const char* p_TilesPath, SDL_Renderer* p_Renderer, const int& p_Width, const int& p_Height) 
	: m_Background(nullptr), m_BackgroundProps(nullptr), m_ForegroundProps(nullptr), m_TileSize(p_TileSize)
{
	m_TilemapTex = TextureUtil::LoadTexture(p_TilesPath, p_Renderer);

	m_Renderer = p_Renderer;

	m_BufferWidth = p_TileSize * p_Width;
	m_BufferHeight = p_TileSize * p_Height;
	m_BufferRect = { 0, 0, p_TileSize * p_Width, p_TileSize * p_Height };
	m_Buffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_BufferWidth, m_BufferHeight);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);

	m_Width = p_Width;
	m_Height = p_Height;

	m_CameraRect = { 0, 0, 320, 180 };
	m_CameraBuffer = SDL_CreateTexture(p_Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 320, 180);
}

Tilemap::~Tilemap() { 
	SDL_DestroyTexture(m_Buffer);
	SDL_DestroyTexture(m_CameraBuffer);
	SDL_DestroyTexture(m_TilemapTex);
	SDL_DestroyTexture(m_Background);
	SDL_DestroyTexture(m_BackgroundProps);
	SDL_DestroyTexture(m_ForegroundProps);
}

void Tilemap::AddLayer(std::vector<unsigned short>&& p_TileMap) {
	m_Layers.push_back(std::move(p_TileMap));
}

void Tilemap::SetBackground(const char* p_TexPath) {
	m_Background = TextureUtil::LoadTexture(p_TexPath, m_Renderer);
}

void Tilemap::AddBackgroundProps(const char* p_TexPath) {
	m_BackgroundProps = TextureUtil::LoadTexture(p_TexPath, m_Renderer);
}

void Tilemap::AddForegroundProps(const char* p_TexPath) {
	m_ForegroundProps = TextureUtil::LoadTexture(p_TexPath, m_Renderer);
}

void Tilemap::SetCollider(std::shared_ptr<unsigned short[]> p_Collider) {
	m_Collider = p_Collider;
}

void Tilemap::RenderToBuffer() const {
	SDL_SetRenderTarget(m_Renderer, m_Buffer);

	SDL_RenderCopy(m_Renderer, m_Background, NULL, NULL);

	SDL_RenderCopy(m_Renderer, m_BackgroundProps, NULL, NULL);

	for (int n = 0; n < m_Layers.size(); n++) {
		RenderTiles(n);
	}

	SDL_RenderCopy(m_Renderer, m_ForegroundProps, NULL, NULL);

	SDL_SetRenderTarget(m_Renderer, NULL);
}

void Tilemap::RenderTiles(const int& p_n) const {
	int k;
	SDL_Rect Temp = { 0, 0, m_TileSize, m_TileSize };

	for (int i = 0; i < m_Height; i++) {
		Temp.y = i * m_TileSize;
		for (int j = 0; j < m_Width; j++) {
			Temp.x = j * m_TileSize;
			k = (i * m_Width) + j;

			if (m_Layers[p_n][k] != 0) {
				SDL_Rect testrect = { (m_Layers[p_n][k] - 1) * m_TileSize, 0, m_TileSize, m_TileSize };
				SDL_RenderCopy(m_Renderer, m_TilemapTex, &testrect, &Temp);
			}
		}
	}
}

void Tilemap::Render() const {
	SDL_SetRenderTarget(m_Renderer, m_CameraBuffer);
	SDL_RenderCopy(m_Renderer, m_Buffer, NULL, &m_BufferRect);

	SDL_SetRenderTarget(m_Renderer, NULL);
	SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
	SDL_RenderCopy(m_Renderer, m_CameraBuffer, &m_CameraRect, NULL);
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