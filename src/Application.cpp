#include "Engine2D.hpp"

// FIGURE OUT HOW TO MAKE COMPONENTS START IF THEY ARE ADDED AT RUN TIME

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
		//m_ColliderDebugger->DebugRender(*m_PlayerCollider, m_DeltaTime, m_Camera);
	}
};
*/


#include "notval/Engine2D.hpp"

class FullscreenToggler : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<FullscreenToggler>(*this);
	}

	void Update() override {
		if (Input.GetKeyDown(SDL_SCANCODE_F11)) {
			Screen.ToggleFullscreen();
		}

		if (!Screen.InFocus()) {
			Input.WaitForEvent();
		}
	}
};

class TestScene : public Scene {
public:
	using Scene::Scene;

	void SetupScene() override {
		auto Background = CreateGameObject("BG");

		auto Layer1 = CreateGameObject("Layer 1");
		Layer1->transform.SetParent(Background);
		auto L1Renderer = Layer1->AddComponent<SpriteRenderer>();
		L1Renderer->SetSprite("assets/medieval/Background/layer_1.png");
		L1Renderer->SetSortingLayer("Background");
		L1Renderer->SetPixelsPerUnit(16);

		auto Layer2 = CreateGameObject("Layer 2");
		Layer2->transform.SetParent(Background);
		Layer2->transform.Translate(Vector2D(0, -2));
		auto L2Renderer = Layer2->AddComponent<SpriteRenderer>();
		L2Renderer->SetSprite("assets/medieval/Background/layer_2.png");
		L2Renderer->SetSortingLayer("Background");
		L2Renderer->SetPixelsPerUnit(16);

		auto PlayerObject = CreateGameObject("Player");
		auto playerRenderer = PlayerObject->AddComponent<SpriteRenderer>();
		playerRenderer->SetSprite("assets/medieval/Characters/knight/idle/idle_knight_1.png");
		playerRenderer->SetSortingLayer("Player");
		playerRenderer->SetPixelsPerUnit(32);

		auto playerBody = PlayerObject->AddComponent<RigidBody>();
		playerBody->SetGravityScale(4);
		auto playerCollider = PlayerObject->AddComponent<BoxCollider>();
		playerCollider->SetTransform(Vector2D(1, 1.5), Vector2D::zero, 0);
		
		//auto groundObject = CreateGameObject("Ground");
		//groundObject->AddComponent<BoxCollider>();
		//groundObject->transform.Translate(Vector2D(0, -4.5));

		CreateGameObject("Fullscreen Toggle")->AddComponent<FullscreenToggler>();

		auto mainCamera = FindObjectByName("Main Camera");
		mainCamera->transform.scale = Vector2D::one * 2;
	}
};

class GameTest : public Engine2D {
public:
	using Engine2D::Engine2D;

	void SetupGame() override {
		RenderingPipeline.SetRendererVsync(true);

		RenderingPipeline.AddSortingLayer("Background");
		RenderingPipeline.AddSortingLayer("Player");

		Physics.SetRenderColliders(true);

		SceneManager.AddScene<TestScene>("Test Scene");
		SceneManager.AddScene<TestScene>("Test Scene 2");
		SceneManager.LoadScene("Test Scene");
	}
};

int main(int argc, char *argv[]) {
	GameTest myGame;

	myGame.InitGame("Engine2D", "assets/characters/idle/madeline.png", 1280, 720);
	myGame.Run();

	return 0;
}