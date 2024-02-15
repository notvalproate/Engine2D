#include "Core.hpp"

Scene::Scene(const std::string_view name, const std::vector<std::string>& avaiableSortingLayers) : name(name), m_Loaded(false) {
    m_CurrentCamera = CreateCamera("Main Camera");
    
    for (const auto& layerName : avaiableSortingLayers) {
        m_SortingLayers.push_back(SortingLayer(layerName));
    }

    m_PhysicsWorld = std::make_unique<b2World>(b2Vec2(0.0f, -9.8f));
}

void Scene::Start() {
    for(auto& gameObject : m_SceneGameObjects) {
        gameObject->Start();
    }
}

void Scene::Update() {
    m_PhysicsWorld->Step(Time.GetDeltaTime(), 6, 2);

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
    for (const auto& layer : m_SortingLayers) {
        for (const auto& gO : layer.m_GameObjectsInLayer) {
            gO->Render();
        }
    }

    Physics.RenderColliders();
}

GameObject* Scene::CreateGameObject() {
    m_SceneGameObjects.push_back(std::unique_ptr<GameObject>(new GameObject(this, m_LatestSceneInstanceID++)));
    return m_SceneGameObjects.back().get();
}

GameObject* Scene::CreateGameObject(const std::string_view goName) {
    m_SceneGameObjects.push_back(std::unique_ptr<GameObject>(new GameObject(goName, this, m_LatestSceneInstanceID++)));
    return m_SceneGameObjects.back().get();
}

Camera* Scene::CreateCamera(const std::string_view camName) {
    auto cameraObj = CreateGameObject(camName);
    auto cameraComp = cameraObj->AddComponent<Camera>();
    m_SceneCameras.push_back(cameraComp);

    return cameraComp;
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

void Scene::SwitchToCamera(const std::string_view cameraName) {
    for (const auto& camera : m_SceneCameras) {
        if (camera->gameObject->name == cameraName) {
            m_CurrentCamera = camera;
            return;
        }
    }
}

void Scene::AddObjectToSortingLayers(GameObject* gameObject) {
    m_SortingLayers[0].m_GameObjectsInLayer.push_back(gameObject);
}

void Scene::RemoveObjectFromSortingLayers(GameObject* gameObject, const std::string_view layerName) {
    auto it = std::find_if(
        m_SortingLayers.begin(),
        m_SortingLayers.end(),
        [&layerName](const SortingLayer& layer) {
            return layer.name == layerName;
        });

    auto& gameObjectsInLayer = (*it).m_GameObjectsInLayer;

    for (std::size_t i = 0; i < gameObjectsInLayer.size(); i++) {
        if (gameObjectsInLayer[i] == gameObject) {
            gameObjectsInLayer.erase(gameObjectsInLayer.begin() + i);
            break;
        }
    }
}

bool Scene::SetSortingLayer(GameObject* gameObject, const std::string_view layerName, const std::string_view previousLayer) {
    auto it = std::find_if(
        m_SortingLayers.begin(),
        m_SortingLayers.end(),
        [&previousLayer](const SortingLayer& layer) {
            return layer.name == previousLayer;
        });

    auto& prevLayerObjects = (*it).m_GameObjectsInLayer;

    for (std::size_t i = 0; i < prevLayerObjects.size(); i++) {
        if (prevLayerObjects[i] == gameObject) {
            prevLayerObjects.erase(prevLayerObjects.begin() + i);
            break;
        }
    }

    auto it2 = std::find_if(
        m_SortingLayers.begin(),
        m_SortingLayers.end(),
        [&layerName](const SortingLayer& layer) {
            return layer.name == layerName;
        });

    if (it2 == m_SortingLayers.end()) {
        m_SortingLayers[0].m_GameObjectsInLayer.push_back(gameObject);
        return false;
    }

    (*it2).m_GameObjectsInLayer.push_back(gameObject);
    return true;
}