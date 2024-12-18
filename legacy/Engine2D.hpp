#pragma once
#include "SDL.h"
#include "tilemap/Tilemap.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "physics/Collider.hpp"
#include <memory>

namespace notval {

	class Engine2D {
	public:
		Engine2D();
		virtual ~Engine2D();

		Engine2D(const Engine2D& other) = delete;
		Engine2D(const Engine2D&& other) = delete;

		Engine2D& operator=(const Engine2D& other) = delete;
		Engine2D& operator=(const Engine2D&& other) = delete;

		virtual void OnUserCreate() {};
		virtual void OnUserEvents() {};
		virtual void OnUserUpdate() {};
		virtual void OnUserRender() {};

		void Init(const char* title, const char* iconpath, const int x, const int y, int windowWidth, int windowHeight);
		void HandleEvents();
		void Update();
		void Render();

		inline bool IsRunning() const { return m_IsRunning; }

	protected:
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Tilemap> m_Level;
		std::unique_ptr<Player> m_Player;
		std::unique_ptr<DynamicCollider2D> m_PlayerCollider;
		std::unique_ptr<ColliderDebugRenderer> m_ColliderDebugger;

		SDL_Renderer* m_Renderer;

		float m_DeltaTime{};

	private:
		bool m_IsRunning{};
		int m_Width{}, m_Height{};
		SDL_Window* m_Window;
		SDL_DisplayMode m_Mode;
		SDL_Event m_Event;

		inline bool InFocus() const { return (SDL_GetWindowFlags(m_Window) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS)); };
	};

}