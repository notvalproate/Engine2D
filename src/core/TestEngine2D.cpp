#include "Core.hpp"

Engine2D::Engine2D() : m_CurrentScene(nullptr), m_IsRunning(true) { }

void Engine2D::Run() {
	while (m_IsRunning) {
		Update();
		Render();
	}
}

void Engine2D::Update() {
	m_CurrentScene->Update();
}

void Engine2D::Render() const {
	m_CurrentScene->Render();
}