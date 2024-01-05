#include "Tilemap.hpp"
#include "../TextureLoader.hpp"

#include "json.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>

#include <sstream>
#include <iomanip>

std::vector<std::string> Tilemap::m_CollisionLayerNames({"world"});

Tilemap::Tilemap(const std::filesystem::path& tilemapPath, SDL_Renderer* renderer)
	: m_Background(nullptr), m_BackgroundProps(nullptr), m_ForegroundProps(nullptr)
{
	m_Renderer = renderer;

	std::ifstream jsonFileStream(tilemapPath);
	auto tilemapJson = nlohmann::json::parse(jsonFileStream);

	m_Width = tilemapJson["width"];
	m_Height = tilemapJson["height"];
	m_TileSize = tilemapJson["tileheight"];

	if (tilemapJson.contains("backgroundcolor")) {
		GetRGBFromHex(tilemapJson["backgroundcolor"]);
	}

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

		auto it = std::find(std::begin(m_CollisionLayerNames), std::end(m_CollisionLayerNames), layerName);

		if (it != std::end(m_CollisionLayerNames)) {
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

	m_BufferRect = { 0, 0, static_cast<int>(m_TileSize * m_Width), static_cast<int>(m_TileSize * m_Height)};
	m_Buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, m_BufferRect.w, m_BufferRect.h);
	SDL_SetTextureBlendMode(m_Buffer, SDL_BLENDMODE_BLEND);
}

Tilemap::~Tilemap() { 
	SDL_DestroyTexture(m_Buffer);
	SDL_DestroyTexture(m_Background);
	SDL_DestroyTexture(m_BackgroundProps);
	SDL_DestroyTexture(m_ForegroundProps);
}

void Tilemap::SetCollisionLayerNames(const std::vector<std::string>& layerNames) {
	m_CollisionLayerNames = layerNames;

	for (auto& name : m_CollisionLayerNames) {
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	}
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
	SDL_SetRenderDrawColor(m_Renderer, m_R, m_G, m_B, m_A);
	SDL_RenderClear(m_Renderer);

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
		unsigned int tileID = layer.data[i];


		if (tileID == 0) {
			continue;
		}

		unsigned int Col = i % layer.width;
		unsigned int Row = i / layer.width;

		SDL_Rect DestRect{ 
			layer.x + Col * m_TileSize,
			layer.y + Row * m_TileSize,
			m_TileSize,
			m_TileSize
		};

		SDL_Rect SrcRect{ NULL, NULL, m_TileSize, m_TileSize};

		double angle = 0;

		SDL_RendererFlip flipFlag = SDL_FLIP_NONE;

		unsigned int tilesetIndex = 0; 
		while (tilesetIndex < m_Tilesets.size() && !m_Tilesets[tilesetIndex]->GetTile(tileID, SrcRect, angle, flipFlag)) { tilesetIndex++; }

		if (tilesetIndex == m_Tilesets.size()) {
			std::cout << "Error in rendering tile ID: " << layer.data[i] << std::endl;
			continue;
		}

		SDL_RenderCopyEx(m_Renderer, m_Tilesets[tilesetIndex]->GetAtlas(), &SrcRect, &DestRect, angle, NULL, flipFlag);
	}
}

void Tilemap::GetRGBFromHex(std::string hexString) {
	if (hexString[0] == '#') {
		hexString.erase(0, 1);
	}
	
	unsigned int RGB;
	unsigned int shift = 0;
	std::istringstream(hexString) >> std::hex >> RGB;

	if (hexString.size() == 8) {
		m_A = RGB & 0xFF;
		shift += 8;
	}
	else {
		m_A = 255;
	}
	m_B = (RGB >> shift) & 0xFF;
	shift += 8;
	m_G = (RGB >> shift) & 0xFF;
	shift += 8;
	m_R = (RGB >> shift) & 0xFF;	
}