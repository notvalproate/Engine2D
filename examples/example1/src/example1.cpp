#include <iostream>

#include "Engine2D.hpp"

namespace engine2d {

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

		rb = gameObject->GetComponent<RigidBody>();
		rb->linearDrag = 1;
		rb->SetMass(1);
		rb->FreezeRotation(true);

		test = gameObject->GetComponent<BoxCollider>();
		mcCursor = new CustomCursor("assets/cursor.png", Vector2D(0, 0));
		boo = koo = false;
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
		
		if (Input.GetKeyDown(SDL_SCANCODE_P)) {
			Destroy(rb);
		}
		
		if (Input.GetKeyDown(SDL_SCANCODE_O)) {
			SceneManager.LoadScene("Test Scene 2");
		}
		
		if (Input.GetKeyDown(SDL_SCANCODE_I)) {
			SceneManager.LoadScene("Test Scene");
		}

		if (Input.GetKeyDown(SDL_SCANCODE_B)) {
			Physics.SetRenderColliders(boo);
			boo = !boo;
		}

		if (Input.GetKeyDown(SDL_SCANCODE_M)) {
			if (!koo) {
				Cursor.SetCursor(*mcCursor);
			}
			else {
				Cursor.SetCursor(SystemCursorType::Arrow);
			}
			koo = !koo;
		}

		std::cout << "Velocity: " << rb->GetVelocity() << std::endl;
	}

	// FIX STAY SINCE EXIT IS CALLED WHEN ANY OF THE SUB-FIXTURES ON A SINGLE COLLIDER CALLS EXIT WHEN IT LEAVES CONTACT, EVEN
	// THOUGH THE COLLIDER AS A WHOLE HAS NOT LEFT CONTACT

	void OnCollisionEnter(const Collision& collision) override {
		std::cout << "Entering Collision with " << collision.collider->gameObject->name << std::endl;
	}

	void OnCollisionExit(const Collision& collision) override {
		std::cout << "Exiting Collision with " << collision.collider->gameObject->name << std::endl;
	}

	void OnTriggerEnter(const Collision& collision) override {
		std::cout << "Triggering with " << collision.collider->gameObject->name << std::endl;
	}

	void OnTriggerExit(const Collision& collision) override {
		std::cout << "Exiting trigger with " << collision.collider->gameObject->name << std::endl;
	}
	
	CustomCursor* mcCursor;
	double radius;
	bool boo, koo;

	Camera* mainCamera;
	RigidBody* rb;
	BoxCollider* test;
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

		auto testObj = CreateGameObject("Test OBJ1", Vector2D(0, 4), 40);
		auto col = testObj->AddComponent<BoxCollider>();
		testObj->AddComponent<RigidBody>();
		col->SetTransform(Vector2D(1, 2), Vector2D(0, 0), 0);

		auto PlayerObject = CreateGameObject("Player");
		PlayerObject->AddComponent<CameraFollower>();

		auto playerRenderer = PlayerObject->AddComponent<SpriteRenderer>();
		playerRenderer->SetSprite("assets/medieval/Characters/knight/idle/idle_knight_1.png");
		playerRenderer->SetSortingLayer("Player");
		playerRenderer->SetPixelsPerUnit(32);

		PlayerObject->AddComponent<Controller>();
		// Comment line above and uncomment line below to use a player controller that uses and tests raycasts
		// PlayerObject->AddComponent<PlayerController>();

		auto rb = PlayerObject->AddComponent<RigidBody>();
		// ISSUE OF INVALID MEM ACCESS SOMEWHERE WHEN CLOSING GAME / DESTRUCTION OF COLLIDER WHEN IN CONTACT / OVERLAP

		// Box Collider Test
		// auto playerBox = PlayerObject->AddComponent<BoxCollider>();
		// playerBox->SetTransform(Vector2D(2, 1), Vector2D(0, 0), 0);

		// Circle Collider Testq
		// auto playerCircle = PlayerObject->AddComponent<CircleCollider>();
		// playerCollider->SetTransform(0.5, Vector2D(0, 0), 0);


		// Capsule Collider Test
		auto playerCapsule = PlayerObject->AddComponent<CapsuleCollider>();
		playerCapsule->SetTransform(Vector2D(0.8, 0.6), CapsuleDirection::Vertical, Vector2D(-0.1, -0.3));
		// playerCapsule->SetTransform(Vector2D(0.8, 0.6), CapsuleDirection::Horizontal, Vector2D(-0.1, -0.3));

		// Polygon Collider Test
		// auto playerPolygon = PlayerObject->AddComponent<PolygonCollider>();
		// playerPolygon->CreatePrimitive(5, Vector2D(1, 1), Vector2D(0, 0));
		// std::vector<Vector2D> points = {
		// 	Vector2D(0.45, -0.29),
		// 	Vector2D(0.82, -1.7),
		// 	Vector2D(3.19, -0.27),
		// 	Vector2D(0.9, 0.34),
		// 	Vector2D(4.0, 3.0),
		// 	Vector2D(1.1, 3.01),
		// 	Vector2D(0.8, 2.4),
		// 	Vector2D(0.88, 1.06),
		// 	Vector2D(-1.69, -0.16)
		// };
		// playerPolygon->SetPoints(points, Vector2D(0, 0));

		// Edge Collider Test
		// auto playerCollider = PlayerObject->AddComponent<EdgeCollider>();
		// playerCollider->SetPoints({ Vector2D(-1, 0), Vector2D(1, 0), Vector2D(1, 1), Vector2D(2, 1), Vector2D(3, 1.5) }, Vector2D(0, 0));

		PhysicsMaterial ice(0, 0.3);

		auto groundObject = CreateGameObject("Ground", Vector2D(0, -5.5), 0);

		auto g1 = CreateGameObject("Ground 1", groundObject, Vector2D(-0.5, 0), 0); 
		auto g1render = g1->AddComponent<SpriteRenderer>();
		g1render->SetSprite("assets/medieval/Tiles/floor_tile_2.png");
		g1render->SetPixelsPerUnit(32);
		g1render->SetSortingLayer("World");
		auto g1collider = g1->AddComponent<BoxCollider>();
		g1collider->SetMaterial(ice);

		auto g2 = CreateGameObject("Ground 2", groundObject, Vector2D(0.5, 0), 0);
		auto g2render = g2->AddComponent<SpriteRenderer>();
		g2render->SetSprite("assets/medieval/Tiles/floor_tile_3.png");
		g2render->SetPixelsPerUnit(32);
		g2render->SetSortingLayer("World");
		auto g2collider = g2->AddComponent<BoxCollider>();
		g2collider->SetMaterial(ice);

		auto g3 = CreateGameObject("Ground 3", groundObject, Vector2D(1.5, 0), 0);
		auto g3render = g3->AddComponent<SpriteRenderer>();
		g3render->SetSprite("assets/medieval/Tiles/floor_tile_4.png");
		g3render->SetPixelsPerUnit(32);
		g3render->SetSortingLayer("World");
		auto g3collider = g3->AddComponent<BoxCollider>();
		g3collider->SetMaterial(ice);

		auto g4 = CreateGameObject("Ground 4", groundObject, Vector2D(-1.5, 0), 0);
		g4->tag = "Spiky";
		auto g4render = g4->AddComponent<SpriteRenderer>();
		g4render->SetSprite("assets/medieval/Tiles/floor_tile_1.png");
		g4render->SetPixelsPerUnit(32);
		g4render->SetSortingLayer("World");
		auto g4collider = g4->AddComponent<BoxCollider>();
		g4collider->SetMaterial(ice);
		g4collider->SetAsTrigger(true);
		
		CreateGameObject("Fullscreen Toggle")->AddComponent<FullscreenToggler>();
	}
};

class GameTest : public engine2d::Engine2D {
public:
	using Engine2D::Engine2D;

	void SetupGame() override {
		RenderingPipeline.SetRendererVsync(true);

		RenderingPipeline.AddSortingLayer("Background");
		RenderingPipeline.AddSortingLayer("Player");
		RenderingPipeline.AddSortingLayer("World");

		Physics.SetRenderColliders(true);
		Physics.SetGravity(Vector2D(0, 0));

		SceneManager.AddScene<TestScene>("Test Scene");
		SceneManager.AddScene<TestScene>("Test Scene 2");
		SceneManager.LoadScene("Test Scene");
	}
};

} // namespace engine2d

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	engine2d::GameTest myGame;

	myGame.InitGame("Engine2D", "assets/cursor.png", 1280, 720);
	myGame.Run();

	return 0;
}
