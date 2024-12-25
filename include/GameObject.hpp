#pragma once

#include <memory>

#include "Object.hpp"
#include "Transform.hpp"
#include "PhysicsHandler.hpp"
#include "SceneHandler.hpp"

class Object;

class GameObject final : public Object {
public:
    ~GameObject();

    bool CompareTag(const std::string_view otherTag) const;

    template<typename T>
    inline constexpr T* AddComponent() {
        AssertComponentIsDerived<T>();

        if (m_Started) {
            return AddComponentDuringUpdate<T>();
        }

        if constexpr (std::is_base_of<Behaviour, T>::value) {
            m_Behaviours.push_back(std::unique_ptr<T>(new T(this)));
            m_Behaviours.back().get()->Awake();
            return static_cast<T*>(m_Behaviours.back().get());
        }

        m_Components.push_back(std::unique_ptr<T>(new T(this)));
        m_Components.back().get()->Awake();
        return static_cast<T*>(m_Components.back().get());
    }

    template<typename First, typename Second, typename... Args>
    inline constexpr void AddComponent() {
        AddComponent<First>();
        AddComponent<Second, Args...>();
    }

    template<typename T>
    inline constexpr T* GetComponent() const {
        AssertComponentIsDerived<T>();

        if constexpr (std::is_base_of<Behaviour, T>::value) {
            for(auto& behaviour : m_Behaviours) {
                auto ptr = dynamic_cast<T*>(behaviour.get());

                if(ptr != nullptr) {
                    return ptr;
                }
            }
            for(auto& behaviour : m_BehavioursStagedForAdding) {
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
            for(auto& component : m_ComponentsStagedForAdding) {
                auto ptr = dynamic_cast<T*>(component.get());

                if(ptr != nullptr) {
                    return ptr;
                }
            }
        }

        return nullptr;
    }

    template<typename T>
    inline constexpr T* GetComponentInChildren() const {
        AssertComponentIsDerived<T>();

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
    inline constexpr T* GetComponentInParent() const {
        AssertComponentIsDerived<T>();

        T* ptr = GetComponent<T>();

        if(ptr == nullptr && transform.m_Parent) {
            ptr = transform.m_Parent->gameObject->GetComponentInParent<T>();
        }

        return ptr;
    }

    template<typename T>
    inline constexpr std::vector<T*> GetComponents() const {
        AssertComponentIsDerived<T>();

        std::vector<T*> components{};

        if constexpr (std::is_base_of<Behaviour, T>::value) {
            for(auto& behaviour : m_Behaviours) {
                auto ptr = dynamic_cast<T*>(behaviour.get());

                if(ptr != nullptr) {
                    components.push_back(ptr);
                }
            }
            for(auto& behaviour : m_BehavioursStagedForAdding) {
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
            for(auto& component : m_ComponentsStagedForAdding) {
                auto ptr = dynamic_cast<T*>(component.get());

                if(ptr != nullptr) {
                    components.push_back(ptr);
                }
            }
        }

        return components;
    }

    template<typename T>
    inline constexpr std::vector<T*> GetComponentsInChildren() const {
        std::vector<T*> components = GetComponents<T>();

        for(auto& childTransform : transform.m_Children) {
            auto componentsInChildren = childTransform->gameObject->GetComponentsInChildren<T>();

            components.insert(std::end(components), std::begin(componentsInChildren), std::end(componentsInChildren));
        }

        return components;
    }

    template<typename T>
    inline constexpr std::vector<T*> GetComponentsInParent() const {
        std::vector<T*> components = GetComponents<T>();

        if(transform.m_Parent) {
            auto componentsInParent = transform.m_Parent->gameObject->GetComponentsInParent<T>();

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
    explicit GameObject(Scene* scene, const uint32_t id);
    explicit GameObject(const std::string_view goName, Scene* scene, const uint32_t id);
    explicit GameObject(const std::string_view goName, Transform* parent, bool instantiateInWorldSpace, Scene* scene, const uint32_t id);

    void Start();
    void Update();
    void Render() const;

    void OnCollisionEnter(const Collision& collision);
    void OnCollisionStay(const Collision& collision);
    void OnCollisionExit(const Collision& collision);

    void OnTriggerEnter(const Collision& collision);
    void OnTriggerStay(const Collision& collision);
    void OnTriggerExit(const Collision& collision);

    void HandleDestructions();

    std::size_t GetComponentIndex(Component* component);
    std::size_t GetBehaviourIndex(Behaviour* behaviour);
    Component* GetComponentByIndex(const std::size_t index);
    Behaviour* GetBehaviourByIndex(const std::size_t index);
    void RemoveComponent(Component* component);
    void RemoveBehaviour(Behaviour* behaviour);

    template<typename T>
    inline constexpr T* AddComponentDuringUpdate() {
        if constexpr (std::is_base_of<Behaviour, T>::value) {
            m_BehavioursStagedForAdding.push_back(std::unique_ptr<T>(new T(this)));
            m_BehavioursStagedForAdding.back().get()->Awake();
            m_BehavioursStagedForAdding.back().get()->Start();

            return static_cast<T*>(m_BehavioursStagedForAdding.back().get());
        }

        m_ComponentsStagedForAdding.push_back(std::unique_ptr<T>(new T(this)));
        m_ComponentsStagedForAdding.back().get()->Awake();
        m_ComponentsStagedForAdding.back().get()->Start();

        return static_cast<T*>(m_ComponentsStagedForAdding.back().get());
    }

    template<typename T>
    inline static void AssertComponentIsDerived() {
        static_assert(
            std::is_base_of<Component, T>::value,
            "Custom Component provided not derived from Component Class"
        );
    }

    bool m_Started;
    bool m_Destroyed;

    std::vector<std::unique_ptr<Component>> m_ComponentsStagedForAdding{};
    std::vector<std::unique_ptr<Behaviour>> m_BehavioursStagedForAdding{};
    std::vector<std::unique_ptr<Component>> m_Components{};
    std::vector<std::unique_ptr<Behaviour>> m_Behaviours{};
    std::vector<Component*> m_ComponentsStagedForDestruction{};
    std::vector<Behaviour*> m_BehavioursStagedForDestruction{};

    friend class Object;
    friend class Scene;
    friend class RenderingHandler;
    friend class PhysicsHandler;
    friend class Collider;
};