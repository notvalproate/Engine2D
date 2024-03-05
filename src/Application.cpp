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

// PLAYER CONTROLLER FROM:
// GitHub: https://github.com/Matthew-J-Spencer/Ultimate-2D-Controller/blob/main/Scripts/PlayerController.cs
// YouTube: https://www.youtube.com/watch?v=3sWTzMsmdx8

class PlayerController : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<PlayerController>(*this);
	}

	struct ScriptableStats {
		float MaxSpeed = 14;
		float Acceleration = 120;
		float GroundDeceleration = 60;
		float AirDeceleration = 30;
		float GroundingForce = -1.5;
		float GrounderDistance = 0.05;
		float JumpPower = 36;
		float MaxFallSpeed = 40;
		float FallAcceleration = 110;
		float JumpEndEarlyGravityModifier = 3;
		float CoyoteTime = .15f;
		float JumpBuffer = .2f;
	};

	struct FrameInput {
		bool JumpDown;
		bool JumpHeld;
		Vector2D Move;
	};

	RigidBody* rb;
	BoxCollider* col;
	FrameInput frameInput;
	Vector2D frameVelocity;
	const ScriptableStats stats;

	double time;

	void Start() override {
		rb = gameObject->GetComponent<RigidBody>();
		col = gameObject->GetComponent<BoxCollider>();

		rb->FreezeRotation(true);

		time = 0;
	}

	void Update() override {
		time += Time.GetDeltaTime();
		GatherInput();

		CheckCollisions();

		HandleJump();
		HandleDirection();
		HandleGravity();

		ApplyMovement();
	}

	void GatherInput() {
		frameInput.JumpDown = Input.GetKeyDown(SDL_SCANCODE_SPACE);
		frameInput.JumpHeld = Input.GetKey(SDL_SCANCODE_SPACE);
		frameInput.Move = Vector2D(Input.GetAxisRaw("Horizontal"), Input.GetAxisRaw("Vertical"));

		if (frameInput.JumpDown) {
			jumpToConsume = true;
			timeJumpWasPressed = time;
		}
	}

	float frameLeftGrounded = 0;
	bool grounded = false;

	void CheckCollisions() {
		Vector2D boxOrigin = col->GetCenter();
		Vector2D boxHalfDimensions = col->GetSize() / 2.0;

		Vector2D topRight = boxOrigin + boxHalfDimensions;
		Vector2D topLeft(boxOrigin.x - boxHalfDimensions.x, topRight.y);

		Vector2D bottomLeft = boxOrigin - boxHalfDimensions;
		Vector2D bottomRight(boxOrigin.x + boxHalfDimensions.x, bottomLeft.y);

		bool topLeftHit = Physics.RayCast(topLeft, Vector2D::up, stats.GrounderDistance).hit;
		bool topRightHit = Physics.RayCast(topRight, Vector2D::up, stats.GrounderDistance).hit;

		bool bottomLeftHit = Physics.RayCast(bottomLeft, Vector2D::down, stats.GrounderDistance).hit;
		bool bottomRightHit = Physics.RayCast(bottomRight, Vector2D::down, stats.GrounderDistance).hit;

		bool groundHit = bottomLeftHit || bottomRightHit; 
		bool ceilingHit = topLeftHit || topRightHit;

		if (ceilingHit) frameVelocity.y = std::min(0.0, frameVelocity.y);

		if (!grounded && groundHit) {
			grounded = true;
			coyoteUsable = true;
			bufferedJumpUsable = true;
			endedJumpEarly = false;
		} 
		else if (grounded && !groundHit) {
			grounded = false;
			frameLeftGrounded = time;
		}
	}

	bool jumpToConsume = false;
	bool bufferedJumpUsable = true;
	bool endedJumpEarly = false;
	bool coyoteUsable = true;
	float timeJumpWasPressed = -FLT_MAX;

	void HandleJump() {
		bool HasBufferedJump = bufferedJumpUsable && (time < (timeJumpWasPressed + stats.JumpBuffer));
		bool CanUseCoyote = coyoteUsable && !grounded && (time < (frameLeftGrounded + stats.CoyoteTime));

		if (!endedJumpEarly && !grounded && !frameInput.JumpHeld && rb->GetVelocity().y > 0) endedJumpEarly = true;

		if (!jumpToConsume && !HasBufferedJump) return;

		if (grounded || CanUseCoyote) ExecuteJump();

		jumpToConsume = false;
	}

	void ExecuteJump() {
		endedJumpEarly = false;
		timeJumpWasPressed = 0;	
		bufferedJumpUsable = false;
		coyoteUsable = false;
		frameVelocity.y = stats.JumpPower;
	}

	void HandleDirection() {
		if (frameInput.Move.x == 0) {
			double deceleration = grounded ? stats.GroundDeceleration : stats.AirDeceleration;
			frameVelocity.x = Math.MoveTowards(frameVelocity.x, 0, deceleration * Time.GetDeltaTime());
		}
		else {
			frameVelocity.x = Math.MoveTowards(frameVelocity.x, frameInput.Move.x * stats.MaxSpeed, stats.Acceleration * Time.GetDeltaTime());
		}
	}

	void HandleGravity() {
		if (grounded && frameVelocity.y <= 0.0) {
			frameVelocity.y = stats.GroundingForce;
		}
		else {
			double inAirGravity = stats.FallAcceleration;
			if (endedJumpEarly && frameVelocity.y > 0) inAirGravity *= stats.JumpEndEarlyGravityModifier;
			frameVelocity.y = Math.MoveTowards(frameVelocity.y, -stats.MaxFallSpeed, inAirGravity * Time.GetDeltaTime());
		}
	}

	void ApplyMovement() {
		rb->SetVelocity(frameVelocity);
	}
};

class CameraFollower : public Behaviour {
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<CameraFollower>(*this);
	}

	void Start() {
		mainCamera = FindObjectByName("Main Camera");
		velocity = Vector2D::zero;
	}

	void LateUpdate() {
		mainCamera->transform.position =
			Vector2D::SmoothDamp(
				mainCamera->transform.position,
				transform->position,
				velocity,
				1,
				Time.GetDeltaTime()
			);
	}

	Vector2D velocity;
	GameObject* mainCamera;
};

class Controller : public Behaviour {
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<Controller>(*this);
	}

	void Start() {
		mainCamera = FindObjectByName("Main Camera")->GetComponent<Camera>();

		Physics.SetGravity(Vector2D::zero);

		mcCursor = new CustomCursor("assets/cursor.png", Vector2D::zero);

		rb = gameObject->GetComponent<RigidBody>();
		rb->SetMass(1);
		rb->drag = 1;
		rb->angularDrag = 1;
	}

	void Update() {
		if (Input.GetKey(SDL_SCANCODE_W)) {
			rb->AddForce(Vector2D::up * 20);
		}

		if (Input.GetKey(SDL_SCANCODE_S)) {
			rb->AddForce(Vector2D::down * 20);
		}

		if (Input.GetKey(SDL_SCANCODE_D)) {
			rb->AddForce(Vector2D::right * 20);
		}

		if (Input.GetKey(SDL_SCANCODE_A)) {
			rb->AddForce(Vector2D::left * 20);
		}
		
		if (Input.GetKey(SDL_SCANCODE_P)) {
			transform->Translate(Vector2D::up * Time.GetDeltaTime());
		}
		
		if (Input.GetKey(SDL_SCANCODE_G)) {
			Cursor.SetCursor(*mcCursor);
		}

		if (Input.GetMouseButton(1)) {
			Vector2D direction(Input.mouseRelX, Input.mouseRelY);
			direction.y = -direction.y;
			rb->AddForce(direction * 2);
		}

		if (Input.GetKeyDown(SDL_SCANCODE_K)) {
			Destroy(test);
			test = nullptr;
		}
		
		if (Input.GetKeyDown(SDL_SCANCODE_J)) {
			test = gameObject->AddComponent<BoxCollider>();
		}
		
		if (Input.GetKeyDown(SDL_SCANCODE_L)) {
			gameObject->scene->SwitchToCamera("Camera 2");
		}

	}
	
	Camera* mainCamera;
	RigidBody* rb;
	BoxCollider* test;
	CustomCursor* mcCursor;
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
		PlayerObject->AddComponent<CameraFollower>();
		PlayerObject->AddComponent<Controller>();
		//PlayerObject->AddComponent<PlayerController>();
		auto playerRenderer = PlayerObject->AddComponent<SpriteRenderer>();
		playerRenderer->SetSprite("assets/medieval/Characters/knight/idle/idle_knight_1.png");
		playerRenderer->SetSortingLayer("Player");
		playerRenderer->SetPixelsPerUnit(32);

		auto playerBody = PlayerObject->AddComponent<RigidBody>();
		auto playerCollider = PlayerObject->AddComponent<CircleCollider>();
		
		auto groundObject = CreateGameObject("Ground", Vector2D(0, -5.5), 0);
		
		auto g1 = CreateGameObject("Ground 1", groundObject, Vector2D(-0.5, 0), 0); 
		auto g1render = g1->AddComponent<SpriteRenderer>();
		g1render->SetSprite("assets/medieval/Tiles/floor_tile_2.png");
		g1render->SetPixelsPerUnit(32);
		g1render->SetSortingLayer("World");
		auto g1collider = g1->AddComponent<BoxCollider>();

		auto g2 = CreateGameObject("Ground 2", groundObject, Vector2D(0.5, 0), 0);
		auto g2render = g2->AddComponent<SpriteRenderer>();
		g2render->SetSprite("assets/medieval/Tiles/floor_tile_3.png");
		g2render->SetPixelsPerUnit(32);
		g2render->SetSortingLayer("World");
		auto g2collider = g2->AddComponent<BoxCollider>();

		auto g3 = CreateGameObject("Ground 3", groundObject, Vector2D(1.5, 0), 0);
		auto g3render = g3->AddComponent<SpriteRenderer>();
		g3render->SetSprite("assets/medieval/Tiles/floor_tile_4.png");
		g3render->SetPixelsPerUnit(32);
		g3render->SetSortingLayer("World");
		auto g3collider = g3->AddComponent<BoxCollider>();

		auto g4 = CreateGameObject("Ground 4", groundObject, Vector2D(-1.5, 0), 0);
		auto g4render = g4->AddComponent<SpriteRenderer>();
		g4render->SetSprite("assets/medieval/Tiles/floor_tile_1.png");
		g4render->SetPixelsPerUnit(32);
		g4render->SetSortingLayer("World");
		auto g4collider = g4->AddComponent<BoxCollider>();
		
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
		SceneManager.LoadScene("Test Scene");
	}
};

int main(int argc, char *argv[]) {
	GameTest myGame;

	myGame.InitGame("Engine2D", "assets/characters/idle/madeline.png", 1280, 720);
	myGame.Run();

	return 0;
}