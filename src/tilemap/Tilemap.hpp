#pragma once
#include "SDL.h"
#include <vector>
#include <memory>
#include <string>

class Tilemap {
public:
	Tilemap(const unsigned short tileSize, const char* tilesPath, SDL_Renderer* renderer, const int width, const int height);
	Tilemap(const char* tilemapPath, const char* tilesPath, SDL_Renderer* renderer);
	~Tilemap();

	Tilemap(const Tilemap& other) = delete;
	Tilemap(const Tilemap&& other) = delete;

	Tilemap& operator=(const Tilemap& other) = delete;
	Tilemap& operator=(const Tilemap&& other) = delete;

	void AddLayer(std::vector<unsigned short>&& tileMap);

	void SetBackground(const char* texPath);
	void AddBackgroundProps(const char* texPath);
	void AddForegroundProps(const char* texPath);

	void SetCollider(std::shared_ptr<unsigned short[]> collider);

	void SaveTilemapAsPng(const char* fileName) const;

	void RenderToBuffer() const;
	void Render() const;

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	std::shared_ptr<unsigned short[]> GetCollider() const { return m_Collider; }

private:
	struct Layer {
		std::string m_Name;
		std::vector<unsigned short> m_Data;
		int m_X, m_Y, m_Width, m_Height;
	};
	std::vector<Layer> m_TestLayers;

	std::vector<std::vector<unsigned short>> m_Layers;
	std::shared_ptr<unsigned short[]> m_Collider;

	unsigned short m_TileSize;
	int m_Width, m_Height;
	int m_BufferWidth, m_BufferHeight;

	SDL_Texture *m_Buffer, *m_CameraBuffer, *m_TilemapTex;
	SDL_Texture *m_Background, *m_BackgroundProps, *m_ForegroundProps;

	SDL_Rect m_CameraRect, m_BufferRect;

	SDL_Renderer *m_Renderer;

	void RenderTiles(const int n) const;
};