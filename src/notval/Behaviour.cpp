#include "Core.hpp"

Behaviour::Behaviour(GameObject* gameObject) : Component(gameObject), enabled(true), isActiveAndEnabled(true), name(&gameObject->name) { }

void Behaviour::AttachGameObject(GameObject* newGameObject) {
    Component::AttachGameObject(newGameObject);
    name = &newGameObject->name;
}

std::unique_ptr<Component> Behaviour::Clone() const {
    return std::make_unique<Behaviour>(*this);
}