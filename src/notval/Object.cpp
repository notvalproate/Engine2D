#include "Core.hpp"

InputHandler Object::Input;
SceneHandler Object::SceneManager;
ScreenHandler Object::Screen;
RenderingHandler Object::RenderingPipeline;
TextureHandler Object::TextureManager;
TimeHandler Object::Time;
PhysicsHandler Object::Physics;
CursorHandler Object::Cursor;
MathModule Object::Math;

void Object::CopyBehaviours(GameObject* newGameObject, GameObject* originalGameObject) {
    for (auto& behaviour : originalGameObject->m_Behaviours) {
        auto componentClone = behaviour.get()->Clone();
        std::unique_ptr<Behaviour> behaviourCasted(static_cast<Behaviour*>(componentClone.release()));

        behaviourCasted.get()->AttachGameObject(newGameObject);

        newGameObject->m_Behaviours.push_back(std::move(behaviourCasted));
    }
}

void Object::CopyComponents(GameObject* newGameObject, GameObject* originalGameObject) {
    for (auto& component : originalGameObject->m_Components) {
        auto componentClone = component.get()->Clone();

        componentClone.get()->AttachGameObject(newGameObject);

        newGameObject->m_Components.push_back(std::move(componentClone));
    }
}

void Object::DestroyChildren(GameObject* gameObject) {
    if (gameObject->transform.m_Children.size() == 0) {
        return;
    }

    for (auto& childTransform : gameObject->transform.m_Children) {
        DestroyChildren(childTransform->gameObject);
    }

    std::vector<std::unique_ptr<GameObject>>& SceneObjects = gameObject->scene->m_SceneGameObjects;

    for (auto& childTransform : gameObject->transform.m_Children) {
        auto it = std::find_if(
            SceneObjects.begin(),
            SceneObjects.end(),
            [&childTransform](const std::unique_ptr<GameObject>& sceneObj) {
                return sceneObj.get() == childTransform->gameObject;
            });

        if (it != SceneObjects.end()) {
            SceneObjects.erase(it);
        }
    }

    gameObject->transform.m_Children.clear();
}

GameObject* Object::Instantiate(GameObject* gameObject) {
    std::string newName = gameObject->name + " ID#" + std::to_string(gameObject->scene->m_LatestSceneInstanceID);
    GameObject* newGameObject = gameObject->scene->CreateGameObject(newName);

    for(auto& childTransform : gameObject->transform.m_Children) {
        Instantiate(childTransform->gameObject, &newGameObject->transform);
    }

    CopyBehaviours(newGameObject, gameObject);
    CopyComponents(newGameObject, gameObject);

    return newGameObject;
}

GameObject* Object::Instantiate(GameObject* gameObject, Transform* parentTransform) {
    GameObject* newGameObject = Instantiate(gameObject);
    newGameObject->transform.SetParent(parentTransform);

    return newGameObject;
}

GameObject* Object::Instantiate(GameObject* gameObject, const Vector2D& position, const double rotation) {
    GameObject* newGameObject = Instantiate(gameObject);
    newGameObject->transform.position = position;
    newGameObject->transform.rotation = rotation;

    return newGameObject;
}

void Object::Destroy(GameObject* gameObject) {
    if (!gameObject) {
        return;
    }

    gameObject->scene->m_StagedForDestruction.push_back(gameObject);
}

void Object::DestroyImmediate(GameObject* gameObject) {
    if (!gameObject) {
        return;
    }

    gameObject->transform.DetachFromParent();

    DestroyChildren(gameObject);

    std::vector<std::unique_ptr<GameObject>>& SceneObjects = gameObject->scene->m_SceneGameObjects;

    for (auto it = SceneObjects.begin(); it != SceneObjects.end(); ++it) {
        if (it->get() == gameObject) {
            SceneObjects.erase(it);
            break;
        }
    }
}

Behaviour* Object::Instantiate(Behaviour* behaviour) {
    std::size_t index = behaviour->gameObject->GetBehaviourIndex(behaviour);
    GameObject* newGameObject = Instantiate(behaviour->gameObject);
    return newGameObject->GetBehaviourByIndex(index);
}

Behaviour* Object::Instantiate(Behaviour* behaviour, Transform* parentTransform) {
    std::size_t index = behaviour->gameObject->GetBehaviourIndex(behaviour);
    GameObject* newGameObject = Instantiate(behaviour->gameObject, parentTransform);
    return newGameObject->GetBehaviourByIndex(index);
}

Behaviour* Object::Instantiate(Behaviour* behaviour, const Vector2D& position, const double rotation) {
    std::size_t index = behaviour->gameObject->GetBehaviourIndex(behaviour);
    GameObject* newGameObject = Instantiate(behaviour->gameObject, position, rotation);
    return newGameObject->GetBehaviourByIndex(index);
}

void Object::Destroy(Behaviour* behaviour) {
    if (!behaviour) {
        return;
    }

    behaviour->gameObject->m_BehavioursStagedForDestruction.push_back(behaviour);
}

void Object::DestroyImmediate(Behaviour* behaviour) {
    if (!behaviour) {
        return;
    }

    try {
        (*behaviour).enabled;
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    behaviour->gameObject->RemoveBehaviour(behaviour);
}

Component* Object::Instantiate(Component* component) {
    std::size_t index = component->gameObject->GetComponentIndex(component);
    GameObject* newGameObject = Instantiate(component->gameObject);
    return newGameObject->GetComponentByIndex(index);
}

Component* Object::Instantiate(Component* component, Transform* parentTransform) {
    std::size_t index = component->gameObject->GetComponentIndex(component);
    GameObject* newGameObject = Instantiate(component->gameObject, parentTransform);
    return newGameObject->GetComponentByIndex(index);
}

Component* Object::Instantiate(Component* component, const Vector2D& position, const double rotation) {
    std::size_t index = component->gameObject->GetComponentIndex(component);
    GameObject* newGameObject = Instantiate(component->gameObject, position, rotation);
    return newGameObject->GetComponentByIndex(index);
}

void Object::Destroy(Component* component) {
    if (!component) {
        return;
    }

    component->gameObject->m_ComponentsStagedForDestruction.push_back(component);
}

void Object::DestroyImmediate(Component* component) {
    if (!component) {
        return;
    }

    component->gameObject->RemoveComponent(component);
}