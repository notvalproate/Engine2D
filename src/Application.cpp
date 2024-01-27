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

#include "Engine2D/Core.hpp"

class Player : public Behaviour {
public:
	using Behaviour::Behaviour;

	std::unique_ptr<Component> Clone() const override {
		return std::make_unique<Player>(*this);
	}

	void Start() {
		myInputString = "";
	}

	void Update() {
		if (Input.GetKeyDown(SDL_SCANCODE_W)) {
			std::cout << gameObject->name << " moved up" << std::endl;
		}
		if (Input.GetKeyDown(SDL_SCANCODE_S)) {
			std::cout << gameObject->name << " moved down" << std::endl;
		}
		if (Input.GetKeyDown(SDL_SCANCODE_A)) {
			std::cout << gameObject->name << " moved left" << std::endl;
		}
		if (Input.GetKeyDown(SDL_SCANCODE_D)) {
			std::cout << gameObject->name << " moved right" << std::endl;
		}
		if (Input.GetMouseButton(SDL_BUTTON_RIGHT)) {
			std::cout << gameObject->name << " holding right click!" << std::endl;
		}
		if (Input.GetMouseButtonDown(SDL_BUTTON_LEFT)) {
			std::cout << gameObject->name << " left clicked!" << std::endl;
		}

		if (Input.GetKeyDown(SDL_SCANCODE_F11)) {
			Screen.SetResolution(1600, 900);
		}
		if (Input.GetKeyDown(SDL_SCANCODE_F10)) {
			Screen.SetResolution(1280, 720);
		}
		
		for (const auto& c : Input.inputString) {
			if (c == '\b' && myInputString.length()) {
				myInputString.resize(myInputString.length() - 1);
			}
			else if (c == '\r') {
				myInputString += '\n';
			}
			else {
				myInputString += c;
			}

			std::cout << myInputString << std::endl;
		}

		if (!Input.mouseScrollDeltaY) {
			return;
		}

		std::cout << Input.mouseScrollDeltaY << std::endl;
	}

	std::string myInputString;
};

class TestScene : public Scene {
public:
	using Scene::Scene;

	void SetupScene() {
		auto PlayerObject = CreateGameObject("Player");
		PlayerObject->AddComponent<Player>();

		std::cout << "Created Player Objects" << std::endl;
	}
};

class GameTest : public Engine2D {
public:
	using Engine2D::Engine2D;

	void SetupGame() override {
		std::cout << "Hi Game started!" << std::endl;
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