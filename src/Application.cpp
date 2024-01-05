#include "Game.hpp"
#include <iostream>
#include <numeric>

class PlatformerGame : public Game {
public:
	void OnUserCreate() override {
		//Setting up Tile's in the tilemap

		Tilemap::SetCollisionLayerNames({ "world" });

		m_Level = std::make_unique<Tilemap>("assets/tilemaps/mario.json", m_Renderer);
		m_Level->RenderToBuffer();
		
		//Player

		SDL_Rect playerRect = { 0, 0, 26, 36 };
		m_Player = std::make_unique<Player>(m_Renderer, "assets/characters/idle/madeline.png", playerRect, 400, 600, 1280);

		//Collider

		m_PlayerCollider = std::make_unique<DynamicCollider2D>(26, 36, 0, 0);
		m_PlayerCollider->SetCollisionLayer(m_Level);
		m_PlayerCollider->SetPlayer(m_Player);

		//m_Camera->SetDimensions(640, 360);
		//m_Camera->SetPosition(0, 400);

		m_ColliderDebugger->SetCollisionLayer(m_Level->GetCollisionLayer());
	}

	void OnUserRender() override {
		m_ColliderDebugger->DebugRender(*m_PlayerCollider, m_DeltaTime, m_Camera);
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