#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "Object.hpp"
#include "components/Camera.hpp"

struct SortingLayer {
    SortingLayer(const std::string_view layerName) : name(layerName), m_GameObjectsInLayer({}) {}

    std::string name;
    std::vector<GameObject*> m_GameObjectsInLayer;
};

class Scene : public Object {
public:
    GameObject* CreateGameObject();
    GameObject* CreateGameObject(const std::string_view goName);
    GameObject* CreateGameObject(const std::string_view goName, const Vector2D& position, double rotation);
    GameObject* CreateGameObject(const std::string_view goName, Transform* parent, bool instantiateInWorldSpace = false);
    GameObject* CreateGameObject(const std::string_view goName, Transform& parent, bool instantiateInWorldSpace = false);
    GameObject* CreateGameObject(const std::string_view goName, GameObject* parent, bool instantiateInWorldSpace = false);
    GameObject* CreateGameObject(const std::string_view goName, Transform* parent, const Vector2D& position, double rotation, bool instantiateInWorldSpace = false);
    GameObject* CreateGameObject(const std::string_view goName, Transform& parent, const Vector2D& position, double rotation, bool instantiateInWorldSpace = false);
    GameObject* CreateGameObject(const std::string_view goName, GameObject* parent, const Vector2D& position, double rotation, bool instantiateInWorldSpace = false);
    Camera* CreateCamera(const std::string_view camName);

    std::vector<GameObject*> FindObjectsByTag(const std::string_view searchTag) const;
    GameObject* FindObjectByName(const std::string_view searchName) const;
    void SwitchToCamera(const std::string_view cameraName);

    std::string name{};

private:
    explicit Scene(const std::string_view name, const std::vector<std::string>& avaiableSortingLayers);

    virtual void SetupScene() = 0;

    void Start();
    void Update();
    void Render() const;

    void AddObjectToSortingLayers(GameObject* gameObject);
    void RemoveObjectFromSortingLayers(GameObject* gameObject, const std::string_view layerName);
    bool SetSortingLayer(GameObject* gameObject, const std::string_view layerName, const std::string_view previousLayer);
    void DestroyCamera(Camera* cam);

    std::unique_ptr<b2World> m_PhysicsWorld;

    std::vector<std::unique_ptr<GameObject>> m_SceneGameObjects{};
    std::vector<GameObject*> m_StagedForDestruction{};
    std::vector<SortingLayer> m_SortingLayers;

    Camera* m_CurrentCamera;
    std::vector<Camera*> m_SceneCameras{};

    uint32_t m_LatestSceneInstanceID{};
    bool m_Loaded;

    friend class Object;
    friend class Engine2D;
    friend class SceneHandler;
    friend class PhysicsHandler;
    friend class SpriteRenderer;
    friend class RigidBody;
    friend class Collider;
    friend class Camera;
};