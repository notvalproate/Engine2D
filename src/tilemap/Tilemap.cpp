#include "Tilemap.hpp"
#include "../TextureLoader.hpp"

#include "json.hpp"
#include <fstream>
#include <iostream>

Tilemap::Tilemap(const unsigned short tileSize, const char* tilesPath, SDL_Renderer* renderer, const int width, const int height) 
	: m_Background(nullptr), m_BackgroundProps(nullptr), m_ForegroundProps(nullptr), m_TileSize(tileSize)
{
	m_TilemapTex = TextureUtil::LoadTexture(tilesPath, renderer);

	m_Renderer = renderer;

	m_Width = width;
	m_Height = height;

	m_BufferRect = { 0, 0, tileSize * width, tileSize * height };
	m_Buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_BufferRect.w, m_BufferRect.h);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
}

Tilemap::Tilemap(const char* tilemapPath, const char* tilesPath, SDL_Renderer* renderer)
	: m_Background(nullptr), m_BackgroundProps(nullptr), m_ForegroundProps(nullptr)
{
	m_TilemapTex = TextureUtil::LoadTexture(tilesPath, renderer);
	m_Renderer = renderer;

	std::ifstream jsonFileStream(tilemapPath);
	auto tilemapJson = nlohmann::json::parse(jsonFileStream);

	m_Width = tilemapJson["width"];
	m_Height = tilemapJson["height"];
	m_TileSize = tilemapJson["tileheight"];

	for (const auto& layer : tilemapJson["layers"]) {
		if (!layer.contains("data")) {
			continue;
		}

		m_TestLayers.push_back({ layer["name"], layer["data"], layer["x"], layer["y"], layer["width"], layer["height"] });
	}

	m_BufferRect = { 0, 0, m_TileSize * m_Width, m_TileSize * m_Height };
	m_Buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_BufferRect.w, m_BufferRect.h);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
}

Tilemap::~Tilemap() { 
	SDL_DestroyTexture(m_Buffer);
	SDL_DestroyTexture(m_TilemapTex);
	SDL_DestroyTexture(m_Background);
	SDL_DestroyTexture(m_BackgroundProps);
	SDL_DestroyTexture(m_ForegroundProps);
}

void Tilemap::AddLayer(std::vector<unsigned short>&& tileMap) {
	m_Layers.push_back(std::move(tileMap));
}

void Tilemap::SetBackground(const char* texPath) {
	m_Background = TextureUtil::LoadTexture(texPath, m_Renderer);
}

void Tilemap::AddBackgroundProps(const char* texPath) {
	m_BackgroundProps = TextureUtil::LoadTexture(texPath, m_Renderer);
}

void Tilemap::AddForegroundProps(const char* texPath) {
	m_ForegroundProps = TextureUtil::LoadTexture(texPath, m_Renderer);
}

void Tilemap::SetCollider(std::shared_ptr<unsigned short[]> collider) {
	m_Collider = collider;
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

void Tilemap::RenderTiles(const int n) const {
	int k;
	SDL_Rect Temp = { 0, 0, m_TileSize, m_TileSize };

	for (int i = 0; i < m_Height; i++) {
		Temp.y = i * m_TileSize;
		for (int j = 0; j < m_Width; j++) {
			Temp.x = j * m_TileSize;
			k = (i * m_Width) + j;

			if (m_Layers[n][k] != 0) {
				SDL_Rect testrect = { (m_Layers[n][k] - 1) * m_TileSize, 0, m_TileSize, m_TileSize };
				SDL_RenderCopy(m_Renderer, m_TilemapTex, &testrect, &Temp);
			}
		}
	}
}

void Tilemap::Render(const std::unique_ptr<Camera>& camera) const {
	camera->RenderToBuffer(m_Buffer, NULL, &m_BufferRect);
}

void Tilemap::SaveTilemapAsPng(const char* fileName) const {
	SDL_Texture* target = SDL_GetRenderTarget(m_Renderer);
	SDL_SetRenderTarget(m_Renderer, m_Buffer);
	int width, height;
	SDL_QueryTexture(m_Buffer, NULL, NULL, &width, &height);
	SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_RenderReadPixels(m_Renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
	IMG_SavePNG(surface, fileName);
	SDL_FreeSurface(surface);
	SDL_SetRenderTarget(m_Renderer, target);
}