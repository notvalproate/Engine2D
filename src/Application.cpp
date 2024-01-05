#include "Game.hpp"
#include <iostream>
#include <numeric>

static constexpr unsigned short TILE_SIZE = 32;

class PlatformerGame : public Game {
public:
	void OnUserCreate() override {
		//Setting up Tile's in the tilemap

		m_Level = std::make_unique<Tilemap>("assets/tilemaps/platformer.json", m_Renderer);
		m_Level->SetCollisionLayerNames({ "WORLD" });
		m_Level->RenderToBuffer();
		
		//Player

		SDL_Rect playerRect = { 0, 0, 26, 36 };
		m_Player = std::make_unique<Player>(m_Renderer, "assets/characters/idle/madeline.png", playerRect, 400, 500, 1280);

		//Collider

		m_PlayerCollider = std::make_unique<DynamicCollider2D>(TILE_SIZE, 26, 36, 0, 0);
		m_PlayerCollider->SetCollisionLayer(m_Level->GetCollisionLayer());
		m_PlayerCollider->SetPlayer(m_Player);

		//m_Camera->SetDimensions(320, 180);
	}
};

int main(int argc, char *argv[]) {
	PlatformerGame game;
	game.Init("Platformer Game", "assets/characters/idle/madeline.png", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720);

	while (game.IsRunning()) {
		game.HandleEvents();
		game.Update();
		game.Render();
	}

	return 0;
}