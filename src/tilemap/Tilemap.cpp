#include "Tilemap.hpp"
#include "../TextureLoader.hpp"

#include "json.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

Tilemap::Tilemap(const std::filesystem::path& tilemapPath, SDL_Renderer* renderer)
	: m_Background(nullptr), m_BackgroundProps(nullptr), m_ForegroundProps(nullptr)
{
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

		std::string layerName = layer["name"];

		m_Layers.push_back({ 
			layerName,
			layer["data"], 
			layer["x"], layer["y"], 
			layer["width"], layer["height"] 
		});

		std::transform(layerName.begin(), layerName.end(), layerName.begin(), ::tolower);

		if (layerName == "world") {
			m_CollisionLayer = m_Layers.back();
		}
	}

	std::string parentDir = tilemapPath.parent_path().string();

	for (const auto& tileset : tilemapJson["tilesets"]) {
		TilesetConfig config = {
			tileset["tileheight"],
			tileset["firstgid"],
			tileset["imagewidth"],
			tileset["imageheight"],
			tileset["margin"],
			tileset["spacing"]
		};

		std::string atlasPath = parentDir + '/' + std::string(tileset["image"]);

		m_Tilesets.push_back(std::make_unique<Tileset>(config, atlasPath.c_str(), m_Renderer));
	}

	m_BufferRect = { 0, 0, m_TileSize * m_Width, m_TileSize * m_Height };
	m_Buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_BufferRect.w, m_BufferRect.h);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
}

Tilemap::~Tilemap() { 
	SDL_DestroyTexture(m_Buffer);
	SDL_DestroyTexture(m_Background);
	SDL_DestroyTexture(m_BackgroundProps);
	SDL_DestroyTexture(m_ForegroundProps);
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

void Tilemap::RenderToBuffer() const {
	SDL_SetRenderTarget(m_Renderer, m_Buffer);

	SDL_RenderCopy(m_Renderer, m_Background, NULL, NULL);

	SDL_RenderCopy(m_Renderer, m_BackgroundProps, NULL, NULL);

	for (const auto& layer : m_Layers) {
		RenderLayer(layer);
	}

	SDL_RenderCopy(m_Renderer, m_ForegroundProps, NULL, NULL);

	SDL_SetRenderTarget(m_Renderer, NULL);
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

void Tilemap::RenderLayer(const Layer& layer) const {
	for (unsigned int i = 0; i < layer.data.size(); i++) {
		unsigned int x = layer.x;
		unsigned int y = layer.y;

		unsigned int Col = i % layer.width;
		unsigned int Row = i / layer.width;

		x += Col * m_TileSize;
		y += Row * m_TileSize;

		SDL_Rect DestRect{ x, y, m_TileSize, m_TileSize };

		SDL_Rect SrcRect{0, 0, m_TileSize, m_TileSize};

		unsigned int j = 0;
		while (j < m_Tilesets.size() && !m_Tilesets[j]->GetTile(layer.data[i], SrcRect)) { j++; }

		if (j == m_Tilesets.size()) {
			std::cout << "Error in rendering tile ID: " << layer.data[i] << std::endl;
			continue;
		}

		SDL_RenderCopy(m_Renderer, m_Tilesets[j]->GetAtlas(), &SrcRect, &DestRect);
	}
}