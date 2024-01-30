#include "Core.hpp"

SceneHandler::SceneHandler() : m_CurrentScene(nullptr) { }

void SceneHandler::LoadScene(std::size_t sceneID) {
	Object::RenderingPipeline.ClearSortingLayers();

	if (sceneID >= m_Scenes.size()) {
		std::cout << "Scene was not loaded! Invalid scene ID" << std::endl;
		return;
	}

	m_CurrentScene = m_Scenes[sceneID].get();
	m_CurrentScene->SetupScene();
}

void SceneHandler::LoadScene(const std::string_view sceneName) {
	Object::RenderingPipeline.ClearSortingLayers();

	auto it = std::find_if(m_Scenes.begin(), m_Scenes.end(), [&sceneName](const auto& scene) { return scene.get()->name == sceneName; });

	if (it == m_Scenes.end()) {
		std::cout << "Scene was not loaded! Invalid scene name" << std::endl;
		return;
	}

	m_CurrentScene = (*it).get();

	if (!m_CurrentScene->m_Loaded) {
		m_CurrentScene->SetupScene();
		m_CurrentScene->m_Loaded = true;
	}
}