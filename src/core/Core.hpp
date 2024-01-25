#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <algorithm>
#include <functional>

class Object;
class Component;
class Behaviour;
class GameObject;
class Scene;

class Vector2D;
class Transform;

class Object {
public:
    static GameObject* Instantiate(GameObject* gameObject);
    static GameObject* Instantiate(GameObject* gameObject, Transform* parentTransform);
    static GameObject* Instantiate(GameObject* gameObject, const Vector2D& position, const double rotation);
    static void Destroy(GameObject* gameObject);
    static void DestroyImmediate(GameObject* gameObject);
    
    static Behaviour* Instantiate(Behaviour* behaviour);
    static Behaviour* Instantiate(Behaviour* behaviour, Transform* parentTransform);
    static Behaviour* Instantiate(Behaviour* behaviour, const Vector2D& position, const double rotation);
    static void Destroy(Behaviour* behaviour);
    static void DestroyImmediate(Behaviour* behaviour);

    static Component* Instantiate(Component* component);
    static Component* Instantiate(Component* component, Transform* parentTransform);
    static Component* Instantiate(Component* component, const Vector2D& position, const double rotation);
    static void Destroy(Component* component);
    static void DestroyImmediate(Component* component);
};


class Vector2D {
public:
    Vector2D() = default;
    Vector2D(double x, double y);

    double x{}, y{};

	double GetMagnitude() const;
    void Normalize();
	void Scale(const int factor);

    static const Vector2D up;
    static const Vector2D down;
    static const Vector2D left;
    static const Vector2D right;
    static const Vector2D one;
    static const Vector2D zero;
};


class Component : public Object {
public:
    virtual ~Component();

    bool CompareTag(const std::string_view otherTag) const;
    std::vector<GameObject*> FindObjectsByTag(const std::string_view searchTag) const;
    GameObject* FindObjectByName(const std::string_view searchName) const;

    GameObject* gameObject;
    Transform* transform;
    std::string* tag;

private:
    Component(GameObject* gameObject);

    virtual void Awake() {};
    virtual void Start() {};
    virtual void Update() {};
    virtual void Render() const {};

    virtual void AttachGameObject(GameObject* newGameObject);
    virtual std::unique_ptr<Component> Clone() const;

    friend class Object;
    friend class GameObject;
    friend class Behaviour;
};


class Behaviour : public Component {
public:
    bool enabled;
    bool isActiveAndEnabled;
    std::string* name;

private:
    Behaviour(GameObject* gameObject);

    void Render() const override final { };

    void AttachGameObject(GameObject* newGameObject) override final;
    std::unique_ptr<Component> Clone() const override;

    friend class Object;
    friend class GameObject;
};

class Transform final {
public:
    Transform(GameObject* gameObject);

    //CHECK WHAT THIS SHOULD BE DOING LATER
    ~Transform();

    Vector2D position{};
    double rotation{};
    Vector2D scale{};

    GameObject* gameObject;

    Transform* parent;
    std::size_t childCount{};

    std::string* tag;
    std::string* name;

    void Translate(const Vector2D& translation);
    void Rotate(const double angle);
    void RotateAround(const Vector2D& point, const double angle);

    void DetachChildren();
    void DetachFromParent();
    Transform* Find(const std::string_view searchName) const;
    Transform* GetChild(const std::size_t index) const;
    size_t GetSiblingIndex() const;
    void SetAsFirstSibling() const;
    void SetAsLastSibling() const;
    void SetSiblingIndex(const std::size_t index) const;

    bool IsChildOf(Transform* parentTransform) const;
    bool IsChildOf(Transform& parentTransform) const;
    bool IsChildOf(GameObject* parentGo) const;

    void SetParent(Transform* parentTransform);
    void SetParent(Transform& parentTransform);
    void SetParent(GameObject* parentGo);

private:
    std::vector<Transform*> m_Children{};

    friend class GameObject;
    friend class Object;
};

class GameObject final : public Object {
public:
    template<typename T>
    T* AddComponent() {
        AssertParametersAreDerived<T>();

        if constexpr (std::is_base_of<Behaviour, T>::value) {
            m_Behaviours.push_back(std::unique_ptr<T>(new T(this)));
            m_Behaviours.back().get()->Awake();
            return static_cast<T*>(m_Behaviours.back().get());
        }

        m_Components.push_back(std::unique_ptr<T>(new T(this)));
        m_Components.back().get()->Awake();
        m_Components.back().get();
    }

    template<typename First, typename Second, typename... Args>
    void AddComponent() {
        AddComponent<First>();
        AddComponent<Second, Args...>();
    }

    template<typename T>
    T* GetComponent() {
        AssertParametersAreDerived<T>();    

        if constexpr (std::is_base_of<Behaviour, T>::value) {
            for(auto& behaviour : m_Behaviours) {
                auto ptr = dynamic_cast<T*>(behaviour.get());

                if(ptr != nullptr) {
                    return ptr;
                }
            }
        }
        else {
            for(auto& component : m_Components) {
                auto ptr = dynamic_cast<T*>(component.get());

                if(ptr != nullptr) {
                    return ptr;
                }
            }
        }

        return nullptr;
    }

    template<typename T>
    T* GetComponentInChildren() {
        AssertParametersAreDerived<T>();

        T* ptr = GetComponent<T>();

        if(ptr == nullptr) {
            for(auto& childTransform : transform.m_Children) {
                ptr = childTransform->gameObject->GetComponentInChildren<T>();

                if(ptr != nullptr) {
                    break;
                }
            }
        }

        return ptr;
    }

    template<typename T>
    T* GetComponentInParent() {
        AssertParametersAreDerived<T>();

        T* ptr = GetComponent<T>();

        if(ptr == nullptr && transform.parent) {
            ptr = transform.parent->gameObject->GetComponentInParent<T>();
        }

        return ptr;
    }

    template<typename T>
    std::vector<T*> GetComponents() {
        AssertParametersAreDerived<T>();

        std::vector<T*> components{};

        if constexpr (std::is_base_of<Behaviour, T>::value) {
            for(auto& behaviour : m_Behaviours) {
                auto ptr = dynamic_cast<T*>(behaviour.get());

                if(ptr != nullptr) {
                    components.push_back(ptr);
                }
            }
        }
        else {
            for(auto& component : m_Components) {
                auto ptr = dynamic_cast<T*>(component.get());

                if(ptr != nullptr) {
                    components.push_back(ptr);
                }
            }
        }

        return components;
    }

    template<typename T>
    std::vector<T*> GetComponentsInChildren() {
        AssertParametersAreDerived<T>();

        std::vector<T*> components = GetComponents<T>();

        for(auto& childTransform : transform.m_Children) {
            auto componentsInChildren = childTransform->gameObject->GetComponentsInChildren<T>();

            components.insert(std::end(components), std::begin(componentsInChildren), std::end(componentsInChildren));
        }

        return components;
    }

    template<typename T>
    std::vector<T*> GetComponentsInParent() {
        AssertParametersAreDerived<T>();

        std::vector<T*> components = GetComponents<T>();

        if(transform.parent) {
            auto componentsInParent = transform.parent->gameObject->GetComponentsInParent<T>();

            components.insert(std::end(components), std::begin(componentsInParent), std::end(componentsInParent));
        }

        return components;
    }

    std::string name;
    std::string tag;
    Transform transform;

    Scene* scene;
    uint32_t m_SceneInstanceID;
private:
    GameObject(Scene* scene, const uint32_t id);
    GameObject(const std::string_view goName, Scene* scene, const uint32_t id);

    void Start();
    void Update();
    void Render() const;
    std::vector<Behaviour*> m_BehavioursStagedForDestruction{};

    std::size_t GetComponentIndex(Component* component);
    std::size_t GetBehaviourIndex(Behaviour* behaviour);
    Component* GetComponentByIndex(const std::size_t index);
    Behaviour* GetBehaviourByIndex(const std::size_t index);
    void RemoveComponent(Component* component);
    void RemoveBehaviour(Behaviour* behaviour);

    template<typename... Args>
    static void AssertParametersAreDerived() {
        static_assert(
            std::conjunction<std::is_base_of<Component, Args>...>::value, 
            "Custom Component provided not derived from Component Class"
        );
    }

    std::vector<std::unique_ptr<Component>> m_Components{};
    std::vector<std::unique_ptr<Behaviour>> m_Behaviours{};
    std::vector<Component*> m_ComponentsStagedForDestruction{};

    friend class Object;
    friend class Scene;
};

class Scene : public Object {
public:
    Scene();
    Scene(const std::string_view name);

    virtual void SetupScene() = 0;

    void Start();
    void Update();
    void Render() const;

    GameObject* CreateGameObject();
    GameObject* CreateGameObject(const std::string_view goName);

    std::vector<GameObject*> FindObjectsByTag(const std::string_view searchTag) const;
    GameObject* FindObjectByName(const std::string_view searchName) const;

    std::string name{};

private:
    std::vector<std::unique_ptr<GameObject>> m_SceneGameObjects{};
    std::vector<GameObject*> m_StagedForDestruction{};

    uint32_t LatestSceneInstanceID{};

    friend class Object;
};