#include <iostream>

#include "SceneHandler.hpp"

namespace engine2d {

SceneHandler::SceneHandler() : m_CurrentScene(nullptr) { }

void SceneHandler::DestroyScenes() {
	m_Scenes.clear();
}

void SceneHandler::LoadScene(std::size_t sceneID) {
	if (sceneID >= m_Scenes.size()) {
		std::cout << "Scene was not loaded! Invalid scene ID" << std::endl;
		return;
	}

	m_CurrentScene = m_Scenes[sceneID].get();

	if (!m_CurrentScene->m_Loaded) {
		m_CurrentScene->SetupScene();
		m_CurrentScene->Start(); 
		m_CurrentScene->m_Loaded = true;
	}
}

void SceneHandler::LoadScene(const std::string_view sceneName) {
	auto it = std::find_if(m_Scenes.begin(), m_Scenes.end(), [&sceneName](const auto& scene) { return scene.get()->name == sceneName; });

	if (it == m_Scenes.end()) {
		std::cout << "Scene was not loaded! Invalid scene name" << std::endl;
		return;
	}

	m_CurrentScene = (*it).get();

	if (!m_CurrentScene->m_Loaded) {
		m_CurrentScene->SetupScene();
		m_CurrentScene->Start();
		m_CurrentScene->m_Loaded = true;
	}
}

} // namespace engine2d