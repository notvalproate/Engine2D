#pragma once

#include <string_view>

#include "Scene.hpp"
#include "RenderingHandler.hpp"
#include "GameObject.hpp"

class SceneHandler {
public:
    template<typename T>
    void AddScene(const std::string_view sceneName) {
        AssertSceneIsDerived<T>();
        m_Scenes.push_back(std::unique_ptr<T>(new T(sceneName, Object::RenderingPipeline.GetAvailableSortingLayers())));
    }

    // WORRY ABOUT HOW ADDITIVE SCENE LOADING IS CURRENLTY THE ONLY WAY. FIND OUT WAY TO NON ADDITIVELY LOAD / UNLOAD THE CURRENTSCENE
    void LoadScene(std::size_t sceneID);
    void LoadScene(const std::string_view sceneName);

    inline Scene* GetCurrentScene() const { return m_CurrentScene; }
    inline Camera* GetCurrentCamera() const { return m_CurrentScene->m_CurrentCamera; }
private:
    SceneHandler();

    void DestroyScenes();

    std::vector<std::unique_ptr<Scene>> m_Scenes{};
    Scene* m_CurrentScene;

    template<typename T>
    static void AssertSceneIsDerived() {
        static_assert(
            std::is_base_of<Scene, T>::value,
            "Scene provided not derived from Scene Class"
        );
    }

    friend class Object;
    friend class Engine2D;
    friend class RenderingHandler;
};