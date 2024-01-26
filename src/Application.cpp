#include "Engine2D.hpp"
#include <iostream>
#include <numeric>

/*
class PlatformerGame : public notval::Engine2D {
public:
	void OnUserCreate() override {
		//Setting up Tilemap
		notval::Tilemap::SetBackgroundLayerNames({ "below player" });
		notval::Tilemap::SetCollisionLayerNames({ "world" });
		notval::Tilemap::SetForegroundLayerNames({ "above player" });

		m_Level = std::make_unique<notval::Tilemap>("assets/tilemaps/tuxemon-town.json", m_Renderer);
		m_Level->RenderToBuffer();
		
		//Player

		SDL_Rect playerRect = { 0, 0, 26, 36 };
		m_Player = std::make_unique<notval::Player>(m_Renderer, "assets/characters/idle/madeline.png", playerRect, 400, 600, 1280);

		//Collider

		m_PlayerCollider = std::make_unique<notval::DynamicCollider2D>(26, 36, 0, 0);
		m_PlayerCollider->SetCollisionLayer(m_Level);
		m_PlayerCollider->SetPlayer(m_Player);

		//m_Camera->SetDimensions(640, 360);
		//m_Camera->SetPosition(0, 400);

		m_ColliderDebugger->SetCollisionLayer(m_Level->GetCollisionLayer());
	}

	void OnUserRender() override {
		m_ColliderDebugger->DebugRender(*m_PlayerCollider, m_DeltaTime, m_Camera);
	}
};*/

#include "core/Core.hpp"

class Player : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<Player>(*this);
	}

	void Update() {
		if (Input.GetKeyDown(SDL_SCANCODE_UP)) {
			std::cout << "Player moved up" << std::endl;
		}
	}
};

class TestScene : public Scene {
public:
	void SetupScene() {
		auto PlayerObject = CreateGameObject("Player");
		PlayerObject->AddComponent<Player>();

		std::cout << "Created Player Object" << std::endl;
	}
};

class GameTest : public Engine2D {
public:
	using Engine2D::Engine2D;

	void SetupGame() override {
		std::cout << "Hi Game started!" << std::endl;
		AddScene<TestScene>();
		LoadScene(0);
	}
};

int main(int argc, char *argv[]) {
	//PlatformerGame game;
	//game.Init("Engine2D", "assets/characters/idle/madeline.png", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720);

	//while (game.IsRunning()) {
		//game.HandleEvents();
		//game.Update();
		//game.Render();
	//}

	GameTest myGame;

	myGame.InitGame("Engine2D", "assets/characters/idle/madeline.png", 1280, 720);
	myGame.Run();

	return 0;
}