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

class Player : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<Player>(*this);
	}

	void Awake() override {
		speed = 10;
		rotationSpeed = 180;
	}

	void Start() override {
		mainCamera = FindObjectByName("Main Camera")->GetComponent<Camera>();
	}

	void Update() override {
		if (Input.GetKey(SDL_SCANCODE_R)) {
			transform->Rotate(rotationSpeed * Time.GetDeltaTime());
		}

		if (Input.GetKey(SDL_SCANCODE_G)) {
			transform->scale = Vector2D(1.0, -1.0);
		}
		if (Input.GetKey(SDL_SCANCODE_F)) {
			transform->scale = Vector2D::one;
		}

		if (Input.GetKeyDown(SDL_SCANCODE_K)) {
			SceneManager.LoadScene("Test Scene 2");
		}
		if (Input.GetKeyDown(SDL_SCANCODE_L)) {
			SceneManager.LoadScene("Test Scene");
		}

		if (Input.mouseScrollDeltaY > 0) {
			mainCamera->transform->scale -= Vector2D(0.02, 0.02);
		}
		if (Input.mouseScrollDeltaY < 0) {
			mainCamera->transform->scale += Vector2D(0.02, 0.02);
		}

		// FIX INSTANTIATE LATER
		//if (Input.GetKeyDown(SDL_SCANCODE_M)) {
			//auto go = Instantiate(go1ref, transform->position, 0);
		//}
	}

	int speed;
	int rotationSpeed;
	Camera* mainCamera;
	GameObject* go1ref;
	std::vector<SpriteRenderer*> renderers;
};

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
		auto backgroundRenderer = Background->AddComponent<SpriteRenderer>();
		backgroundRenderer->SetSprite("assets/backgrounds/BG.png");
		backgroundRenderer->SetSortingLayer("Background");
		backgroundRenderer->SetPixelsPerUnit(8);

		auto PlayerObject = CreateGameObject("Player");
		PlayerObject->AddComponent<Player, BoxCollider>();
		auto playerRenderer = PlayerObject->AddComponent<SpriteRenderer>();
		playerRenderer->SetSprite("assets/characters/idle/madeline.png");
		playerRenderer->SetSortingLayer("Player");
		playerRenderer->SetPixelsPerUnit(8);
		
		auto PlayerObject2 = CreateGameObject("Player");
		PlayerObject2->AddComponent<Player, BoxCollider>();
		auto playerRenderer2 = PlayerObject->AddComponent<SpriteRenderer>();
		playerRenderer2->SetSprite("assets/characters/idle/madeline.png");
		playerRenderer2->SetSortingLayer("Player");
		playerRenderer2->SetPixelsPerUnit(8);

		CreateGameObject("Fullscreen Toggle")->AddComponent<FullscreenToggler>();
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