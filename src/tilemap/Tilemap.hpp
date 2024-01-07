#pragma once
#include "SDL.h"
#include "../Camera.hpp"
#include "Tileset.hpp"
#include <vector>
#include <memory>
#include <string>
#include <filesystem>

namespace notval {

	class Tilemap {
	public:
		Tilemap(const std::filesystem::path& tilemapPath, SDL_Renderer* renderer);
		~Tilemap();

		Tilemap(const Tilemap& other) = delete;
		Tilemap(const Tilemap&& other) = delete;

		Tilemap& operator=(const Tilemap& other) = delete;
		Tilemap& operator=(const Tilemap&& other) = delete;

		struct Layer {
			std::string name;
			std::vector<unsigned int> data;
			int x, y, width, height;
		};

		static void SetBackgroundLayerNames(const std::vector<std::string>& layerNames);
		static void SetCollisionLayerNames(const std::vector<std::string>& layerNames);
		static void SetForegroundLayerNames(const std::vector<std::string>& layerNames);

		void SetBackground(const char* texPath);
		void AddBackgroundProps(const char* texPath);
		void AddForegroundProps(const char* texPath);

		void SaveTilemapAsPng(const char* fileName) const;

		void RenderToBuffer() const;
		void Render(const std::unique_ptr<Camera>& camera) const;

		inline const Layer* GetCollisionLayer() const { return &m_CollisionLayer; }
		inline unsigned int GetTileSize() const { return m_TileSize; }
	private:
		std::vector<Layer> m_Layers;
		std::vector<std::unique_ptr<Tileset>> m_Tilesets;

		static std::vector<std::string> m_BackgroundLayerNames;
		static std::vector<std::string> m_CollisionLayerNames;
		static std::vector<std::string> m_ForegroundLayerNames;
		Layer m_CollisionLayer;

		unsigned short m_TileSize;
		unsigned int m_Width, m_Height;
		uint8_t m_R{}, m_G{}, m_B{}, m_A{};

		SDL_Texture* m_Buffer;
		SDL_Rect m_BufferRect;

		SDL_Texture* m_Background, * m_BackgroundProps, * m_ForegroundProps;

		SDL_Renderer* m_Renderer;

		void RenderLayer(const Layer& layer) const;
		void GetRGBFromHex(std::string hexString);

		static void SetLayerNamesFor(std::vector<std::string>& layerNamesList, const std::vector<std::string>& layerNames);
	};

}