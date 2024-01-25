#include "Core.hpp"

Scene::Scene() { }

Scene::Scene(const std::string_view name) : name(name) { }

void Scene::Start() {
    for(auto& gameObject : m_SceneGameObjects) {
        gameObject->Start();
    }
}

void Scene::Update() {
    for(auto& gameObject : m_SceneGameObjects) {
        gameObject->Update();
    }

    if(m_StagedForDestruction.size()) {
        for(auto& gO : m_StagedForDestruction) {
            GameObject::DestroyImmediate(gO);
        }
        m_StagedForDestruction.clear();
    }
}

void Scene::Render() const {
    for(auto& gameObject : m_SceneGameObjects) {
        gameObject->Render();
    }
}

GameObject* Scene::CreateGameObject() {
    m_SceneGameObjects.push_back(std::unique_ptr<GameObject>(new GameObject(this, LatestSceneInstanceID++)));
    return m_SceneGameObjects.back().get();
}

GameObject* Scene::CreateGameObject(const std::string_view goName) {
    m_SceneGameObjects.push_back(std::unique_ptr<GameObject>(new GameObject(goName, this, LatestSceneInstanceID++)));
    return m_SceneGameObjects.back().get();
}

GameObject* Scene::FindObjectByName(const std::string_view searchName) const {
    for(auto& object : m_SceneGameObjects) {
        if(object.get()->name == searchName) {
            return object.get();
        }
    }

    return nullptr;
}

std::vector<GameObject*> Scene::FindObjectsByTag(const std::string_view searchTag) const {
    std::vector<GameObject*> objects{};
    
    for(auto& object : m_SceneGameObjects) {
        if(object.get()->tag == searchTag) {
            objects.push_back(object.get());
        }
    }

    return objects;
}