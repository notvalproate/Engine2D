#include "Engine2D.hpp"

// FIGURE OUT HOW TO RELOAD A SCENE AND CONTINUE WHERE LEFT OFF WITH SORTING LAYERS

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
		speed = 360;
		rotationSpeed = 180;
	}

	void Update() override {
		float moveX = 0, moveY = 0;

		if (Input.GetKey(SDL_SCANCODE_W)) {
			moveY = -1;
		}
		if (Input.GetKey(SDL_SCANCODE_S)) {
			moveY = 1;
		}
		if (Input.GetKey(SDL_SCANCODE_A)) {
			moveX = -1;
		}
		if (Input.GetKey(SDL_SCANCODE_D)) {
			moveX = 1;
		}

		Vector2D velocity(moveX, moveY);
		velocity.Normalize();

		transform->Translate(velocity * speed * Time.GetFixedDeltaTime());

		if (Input.GetKey(SDL_SCANCODE_R)) {
			transform->RotateAround(Vector2D(640, 360), rotationSpeed * Time.GetDeltaTime());
		}
		if (Input.GetKey(SDL_SCANCODE_T)) {
			transform->Rotate(rotationSpeed * Time.GetDeltaTime());
		}

		if (Input.GetKey(SDL_SCANCODE_G)) {
			transform->scale = Vector2D::one * -5;
		}
		if (Input.GetKey(SDL_SCANCODE_F)) {
			transform->scale = Vector2D::one * 5;
		}

		if (Input.GetKeyDown(SDL_SCANCODE_K)) {
			SceneManager.LoadScene("Test Scene 2");
		}
		if (Input.GetKeyDown(SDL_SCANCODE_L)) {
			SceneManager.LoadScene("Test Scene");
		}
	}

	int speed;
	int rotationSpeed;
};

class PlayerChild : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<PlayerChild>(*this);
	}

	void Awake() override {
		speed = 360;
		rotationSpeed = 180;
	}

	void Update() override {
		float moveX = 0, moveY = 0;

		if (Input.GetKey(SDL_SCANCODE_UP)) {
			moveY = -1;
		}
		if (Input.GetKey(SDL_SCANCODE_DOWN)) {
			moveY = 1;
		}
		if (Input.GetKey(SDL_SCANCODE_LEFT)) {
			moveX = -1;
		}
		if (Input.GetKey(SDL_SCANCODE_RIGHT)) {
			moveX = 1;
		}

		Vector2D velocity(moveX, moveY);
		velocity.Normalize();

		transform->Translate(velocity * speed * Time.GetDeltaTime());

		if (Input.GetKey(SDL_SCANCODE_Y)) {
			transform->RotateAround(Vector2D(640, 360), rotationSpeed * Time.GetDeltaTime());
		}
		if (Input.GetKey(SDL_SCANCODE_U)) {
			transform->Rotate(rotationSpeed * Time.GetDeltaTime());
		}
	}

	int speed;
	int rotationSpeed;
};

class FPSCounter : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<FPSCounter>(*this);
	}

	void Awake() override {
		fps = Time.GetFixedFramerate();
		count = 0;
		sum = 0;
	}

	void Update() override {
		count++;
		sum += Time.GetFramerate();

		if (count == fps) {
			std::cout << "FPS: " << sum / count << std::endl;
			count = 0;
			sum = 0;
		}
	}

	int fps;
	int count;
	int sum;
};

class TestScene : public Scene {
public:
	using Scene::Scene;

	void SetupScene() override {
		auto Background = CreateGameObject("BG");
		auto backgroundRenderer = Background->AddComponent<SpriteRenderer>();
		backgroundRenderer->SetSprite("assets/backgrounds/BG.png");
		backgroundRenderer->SetSortingLayer("Background");
		Background->transform.scale = Vector2D::one * 4;
		Background->transform.Translate(Vector2D(Screen.GetScreenWidth() / 2, Screen.GetScreenHeight() / 2));


		auto PlayerObject = CreateGameObject("Player");
		PlayerObject->AddComponent<Player>();
		auto playerRenderer = PlayerObject->AddComponent<SpriteRenderer>();
		playerRenderer->SetSprite("assets/characters/idle/madeline.png");
		playerRenderer->SetSortingLayer("Player");
		PlayerObject->transform.scale = Vector2D::one * 5;


		auto PlayerObject2 = CreateGameObject("Player 2");
		PlayerObject2->AddComponent<PlayerChild>();
		auto player2Renderer = PlayerObject2->AddComponent<SpriteRenderer>();
		player2Renderer->SetSprite("assets/characters/idle/madeline.png");
		player2Renderer->SetSortingLayer("Player Child");
		PlayerObject2->transform.scale = Vector2D::one * 2;
		PlayerObject2->transform.SetParent(PlayerObject);
		PlayerObject2->transform.Translate(Vector2D(40, 0));


		//auto fpsCounter = CreateGameObject("FPS Counter");
		//fpsCounter->AddComponent<FPSCounter>();
	}
};

class GameTest : public Engine2D {
public:
	using Engine2D::Engine2D;

	void SetupGame() override {
		RenderingPipeline.AddSortingLayer("Background");
		RenderingPipeline.AddSortingLayer("Player");
		RenderingPipeline.AddSortingLayer("Player Child");

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