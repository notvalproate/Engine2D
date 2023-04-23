#pragma once
#include "SDL.h"
#include <vector>
#include <string>
#include "Player.hpp"

class Tilemap {
public:
	Tilemap(const char* p_TilesPath, const std::vector<SDL_Rect>& p_Tiles, SDL_Renderer* p_Renderer, const int& p_Width, const int& p_Height);
	~Tilemap();

	void AddLayer(const std::string& p_TileMapType, unsigned short* p_TileMap);
	void SetBackground(const std::string& p_TexPath);
	void AddProps(const std::string& p_TexPath);
	void SetCollider(unsigned short* p_Collider);

	void SaveTilemap(const char* file_name);

	void RenderToBuffer();
	void Render();

	unsigned short* GetCollider() { return m_Collider; }
	int GetWidth() { return m_Width; }
private:
	std::vector<unsigned short*> m_TileLayers;
	std::vector<SDL_Rect> m_Tiles;
	std::string m_Background;
	std::string m_Props;

	unsigned short* m_Collider;
	int m_Width, m_Height;

	SDL_Texture *m_Buffer, *m_TilemapTex;
	SDL_Renderer* m_Renderer;
	SDL_Rect m_Camera;

	void RenderTiles(const int& p_n);
};