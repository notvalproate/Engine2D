#pragma once
#include "SDL.h"
#include "../Camera.hpp"
#include "Tileset.hpp"
#include <vector>
#include <memory>
#include <string>
#include <filesystem>

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
		std::vector<unsigned short> data;
		int x, y, width, height;
	};

	void SetBackground(const char* texPath);
	void AddBackgroundProps(const char* texPath);
	void AddForegroundProps(const char* texPath);

	void SetCollider(std::shared_ptr<unsigned short[]> collider);

	void SaveTilemapAsPng(const char* fileName) const;

	void RenderToBuffer() const;
	void Render(const std::unique_ptr<Camera>& camera) const;

	std::shared_ptr<unsigned short[]> GetCollider() const { return m_Collider; }

	const Layer* GetTestCollider() const { return &m_TestCollider; }

private:
	std::vector<Layer> m_Layers;
	std::vector<std::unique_ptr<Tileset>> m_Tilesets;
	Layer m_TestCollider;
	
	std::shared_ptr<unsigned short[]> m_Collider;

	unsigned short m_TileSize;
	int m_Width, m_Height;

	SDL_Texture *m_Buffer;
	SDL_Texture *m_Background, *m_BackgroundProps, *m_ForegroundProps;

	SDL_Rect m_BufferRect;

	SDL_Renderer *m_Renderer;

	void RenderLayer(const Layer& layer) const;
};