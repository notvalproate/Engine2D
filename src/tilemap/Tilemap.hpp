#pragma once
#include "SDL.h"
#include <vector>
#include <string>

class Tilemap {
public:
	Tilemap(const unsigned short p_TileSize, const char* p_TilesPath, const std::vector<SDL_Rect>& p_Tiles, SDL_Renderer* p_Renderer, const int& p_Width, const int& p_Height);
	~Tilemap();

	void AddLayer(unsigned short* p_TileMap);
	void SetBackground(const std::string& p_TexPath);
	void AddBackgroundProps(const std::string& p_TexPath);
	void AddForegroundProps(const std::string& p_TexPath);
	void SetCollider(unsigned short* p_Collider);

	void SaveTilemapAsPng(const char* file_name) const;

	void RenderToBuffer();
	void Render() const;


	unsigned short* GetCollider() const { return m_Collider; }
	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height; }
private:
	std::vector<unsigned short*> m_TileLayers;
	std::vector<SDL_Rect> m_Tiles;

	std::string m_Background;
	std::string m_BackgroundProps, m_ForegroundProps;

	unsigned short* m_Collider;
	unsigned short m_TileSize;
	int m_Width, m_Height;
	int m_BufferWidth, m_BufferHeight;

	SDL_Texture *m_Buffer, *m_CameraBuffer, *m_TilemapTex;
	SDL_Renderer* m_Renderer;
	SDL_Rect m_CameraRect, m_BufferRect;

	void RenderTiles(const int& p_n);
};