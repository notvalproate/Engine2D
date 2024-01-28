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

#include "notval/Engine2D.hpp"

class Player : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<Player>(*this);
	}

	void Awake() override {
		speed = 360;
	}

	void Update() override {
		if (Input.GetKey(SDL_SCANCODE_W)) {
			transform->position.y -= speed * Time.GetDeltaTime();
		}
		if (Input.GetKey(SDL_SCANCODE_S)) {
			transform->position.y += speed * Time.GetDeltaTime();
		}
		if (Input.GetKey(SDL_SCANCODE_A)) {
			transform->position.x -= speed * Time.GetDeltaTime();
		}
		if (Input.GetKey(SDL_SCANCODE_D)) {
			transform->position.x += speed * Time.GetDeltaTime();
		}
	}

	int speed;
};

class TestScene : public Scene {
public:
	using Scene::Scene;

	void SetupScene() override {
		auto PlayerObject = CreateGameObject("Player");

		PlayerObject->AddComponent<Player>();
		PlayerObject->AddComponent<SpriteRenderer>()->SetSprite("assets/characters/idle/madeline.png");
		PlayerObject->transform.scale = Vector2D::one * 5;
	}
};

class GameTest : public Engine2D {
public:
	using Engine2D::Engine2D;

	void SetupGame() override {
		SceneManager.AddScene<TestScene>("Test Scene");
		SceneManager.LoadScene("Test Scene");
	}
};

int main(int argc, char *argv[]) {
	GameTest myGame;

	myGame.InitGame("Engine2D", "assets/characters/idle/madeline.png", 1280, 720);
	myGame.Run();

	return 0;
}