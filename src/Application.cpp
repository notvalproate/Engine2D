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

class PlayerController : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<PlayerController>(*this);
	}

	struct FrameInput
	{
		bool JumpDown;
		bool JumpHeld;
		Vector2D Move;
	};

	RigidBody* rb;
	BoxCollider* col;
	FrameInput frameInput;
	Vector2D frameVelocity;

	double time;

	void Start() override {
		mainCamera = FindObjectByName("Main Camera");
		rb = gameObject->GetComponent<RigidBody>();
		col = gameObject->GetComponent<BoxCollider>();

		rb->FreezeRotation(true);
		rb->SetGravityScale(0);
	}

	void Update() override {
		time += Time.GetDeltaTime();
		GatherInput();

		CheckCollisions();

		HandleJump();
		HandleDirection();
		HandleGravity();

		ApplyMovement();

		mainCamera->transform.position = transform->position;
	}

	void GatherInput() {
		frameInput.JumpDown = Input.GetKeyDown(SDL_SCANCODE_SPACE);
		frameInput.JumpHeld = Input.GetKey(SDL_SCANCODE_SPACE);
		frameInput.Move = Vector2D(Input.GetAxisRaw("Horizontal"), Input.GetAxisRaw("Vertical"));

		if (frameInput.JumpDown) {
			JumpToConsume = true;
			timeJumpWasPressed = time;
		}
	}

	float frameLeftGround = 0;
	bool grounded = true;

	void CheckCollisions() {
		bool groundHit = false;
		bool ceilingHit = false;

		if (ceilingHit) frameVelocity.y = std::min(0.0, frameVelocity.y);

		if (!grounded && groundHit) {
			grounded = true;

		}
	}

	bool jumpToConsume;
	bool bufferedJumpUsable;
	bool endedJumpEarly;
	bool coyoteUsable;
	float timeJumpWasPressed;

	void HandleJump() {

	}

	void HandleDirection() {

	}

	void HandleGravity() {

	}

	void ApplyMovement() {

	}

	bool JumpToConsume;
	double timeJumpWasPressed;

	GameObject* mainCamera;
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
		PlayerObject->AddComponent<PlayerController>();
		auto playerRenderer = PlayerObject->AddComponent<SpriteRenderer>();
		playerRenderer->SetSprite("assets/medieval/Characters/knight/idle/idle_knight_1.png");
		playerRenderer->SetSortingLayer("Player");
		playerRenderer->SetPixelsPerUnit(32);

		auto playerBody = PlayerObject->AddComponent<RigidBody>();
		auto playerCollider = PlayerObject->AddComponent<BoxCollider>();
		playerCollider->SetTransform(Vector2D(2, 2), Vector2D::zero, 0);
		

		auto groundObject = CreateGameObject("Ground");

		auto g1 = CreateGameObject("Ground 1");
		g1->transform.SetParent(groundObject);
		g1->transform.Translate(Vector2D(-0.5, 0));
		auto g1collider = g1->AddComponent<BoxCollider>();
		auto g1render = g1->AddComponent<SpriteRenderer>();
		g1render->SetSprite("assets/medieval/Tiles/floor_tile_2.png");
		g1render->SetPixelsPerUnit(32);
		g1render->SetSortingLayer("World");

		auto g2 = CreateGameObject("Ground 2");
		g2->transform.SetParent(groundObject);
		g2->transform.Translate(Vector2D(0.5, 0));
		auto g2collider = g2->AddComponent<BoxCollider>();
		auto g2render = g2->AddComponent<SpriteRenderer>();
		g2render->SetSprite("assets/medieval/Tiles/floor_tile_3.png");
		g2render->SetPixelsPerUnit(32);
		g2render->SetSortingLayer("World");

		auto g3 = CreateGameObject("Ground 3");
		g3->transform.SetParent(groundObject);
		g3->transform.Translate(Vector2D(1.5, 0));
		auto g3collider = g3->AddComponent<BoxCollider>();
		auto g3render = g3->AddComponent<SpriteRenderer>();
		g3render->SetSprite("assets/medieval/Tiles/floor_tile_4.png");
		g3render->SetPixelsPerUnit(32);
		g3render->SetSortingLayer("World");

		auto g4 = CreateGameObject("Ground 2");
		g4->transform.SetParent(groundObject);
		g4->transform.Translate(Vector2D(-1.5, 0));
		auto g4collider = g4->AddComponent<BoxCollider>();
		auto g4render = g4->AddComponent<SpriteRenderer>();
		g4render->SetSprite("assets/medieval/Tiles/floor_tile_1.png");
		g4render->SetPixelsPerUnit(32);
		g4render->SetSortingLayer("World");
		

		groundObject->transform.Translate(Vector2D(0, -5.5));

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
		RenderingPipeline.AddSortingLayer("World");

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