#include "Core.hpp"

#include <ranges>

GameObject::GameObject(Scene* scene, const uint32_t id) 
    : name({}), tag({}), transform(this), scene(scene), m_SceneInstanceID(id), m_Started(false) { }

GameObject::GameObject(const std::string_view goName, Scene* scene, const uint32_t id)
    : name(goName), tag({}), transform(this), scene(scene), m_SceneInstanceID(id) { }

GameObject::GameObject(const std::string_view goName, Transform* parent, bool instantiateInWorldSpace, Scene* scene, const uint32_t id)
    : name(goName), tag({}), transform(this), scene(scene), m_SceneInstanceID(id) 
{ 
    transform.SetParent(parent, instantiateInWorldSpace);
}

void GameObject::Start() {
    m_Started = true;

    for (auto& component : m_Components) {
        component->Start();
    }

    for (auto& behaviour : m_Behaviours) {
        behaviour->Start();
    }
}

void GameObject::Update() {
    for (auto& component : m_Components) {
        component->Update();
    }

    for (auto& behaviour : m_Behaviours) {
        if(behaviour->enabled) {
            behaviour->Update();
        }
    }

    if (m_ComponentsStagedForDestruction.size()) {
        for (auto& component : m_ComponentsStagedForDestruction) {
            RemoveComponent(component);
        }
        m_ComponentsStagedForDestruction.clear();
    }

    if(m_BehavioursStagedForDestruction.size()) {
        for(auto& behaviour : m_BehavioursStagedForDestruction) {
            RemoveBehaviour(behaviour);
        }
        m_BehavioursStagedForDestruction.clear();
    }

    if (m_ComponentsStagedForAdding.size()) {
        m_Components.insert(m_Components.end(),
            std::make_move_iterator(m_ComponentsStagedForAdding.begin()),
            std::make_move_iterator(m_ComponentsStagedForAdding.end())
        );
        m_ComponentsStagedForAdding.clear();
    }

    if (m_BehavioursStagedForAdding.size()) {
        m_Behaviours.insert(m_Behaviours.end(),
            std::make_move_iterator(m_BehavioursStagedForAdding.begin()),
            std::make_move_iterator(m_BehavioursStagedForAdding.end())
        );
        m_BehavioursStagedForAdding.clear();
    } 
}

void GameObject::Render() const {
    for(const auto& component : m_Components) {
        component->Render();
    }
}

std::size_t GameObject::GetComponentIndex(Component* component) {
    std::size_t i = 0;

    for(const auto& comp : m_Components) {
        if(component == comp.get()) {
            return i;
        }
        i++;
    }

    return i;
}

std::size_t GameObject::GetBehaviourIndex(Behaviour* behaviour) {
    std::size_t i = 0;

    for(const auto& comp : m_Components) {
        if(behaviour == comp.get()) {
            return i;
        }
        i++;
    }

    return i;
}

Component* GameObject::GetComponentByIndex(const std::size_t index) {
    if(index >= m_Components.size()) {
        return nullptr;
    }

    return m_Components[index].get();
}

Behaviour* GameObject::GetBehaviourByIndex(const std::size_t index) {
    if(index >= m_Behaviours.size()) {
        return nullptr;
    }

    return m_Behaviours[index].get();
}


void GameObject::RemoveComponent(Component* component) {
    std::size_t index = GetComponentIndex(component);
    m_Components.erase(m_Components.begin() + index);
}

void GameObject::RemoveBehaviour(Behaviour* behaviour) {
    std::size_t index = GetBehaviourIndex(behaviour);
    m_Behaviours.erase(m_Behaviours.begin() + index);
}