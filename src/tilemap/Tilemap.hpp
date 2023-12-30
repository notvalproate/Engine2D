#pragma once
#include "SDL.h"
#include <vector>
#include <memory>

class Tilemap {
public:
	Tilemap(const unsigned short p_TileSize, const char* p_TilesPath, SDL_Renderer* p_Renderer, const int& p_Width, const int& p_Height);
	~Tilemap();

	Tilemap(const Tilemap& other) = delete;
	Tilemap(const Tilemap&& other) = delete;

	Tilemap& operator=(const Tilemap& other) = delete;
	Tilemap& operator=(const Tilemap&& other) = delete;

	void AddLayer(std::vector<unsigned short>&& p_TileMap);

	void SetBackground(const char* p_TexPath);
	void AddBackgroundProps(const char* p_TexPath);
	void AddForegroundProps(const char* p_TexPath);

	void SetCollider(std::shared_ptr<unsigned short[]> p_Collider);

	void SaveTilemapAsPng(const char* file_name) const;

	void RenderToBuffer() const;
	void Render() const;

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
	std::shared_ptr<unsigned short[]> GetCollider() const { return m_Collider; }

private:
	std::vector<std::vector<unsigned short>> m_Layers;
	std::shared_ptr<unsigned short[]> m_Collider;

	const unsigned short m_TileSize;
	int m_Width, m_Height;
	int m_BufferWidth, m_BufferHeight;

	SDL_Texture *m_Buffer, *m_CameraBuffer, *m_TilemapTex;
	SDL_Texture *m_Background, *m_BackgroundProps, *m_ForegroundProps;

	SDL_Rect m_CameraRect, m_BufferRect;

	SDL_Renderer *m_Renderer;

	void RenderTiles(const int& p_n) const;
};