#pragma once

#include <string_view>
#include <memory>

#include "Object.hpp"

namespace engine2d {

class Component : public Object {
public:
    virtual ~Component();

    bool CompareTag(const std::string_view otherTag) const;
    std::vector<GameObject*> FindObjectsByTag(const std::string_view searchTag) const;
    GameObject* FindObjectByName(const std::string_view searchName) const;

    GameObject* gameObject;
    Transform* transform;
    std::string* tag;

protected:
    explicit Component(GameObject* gameObject);
    virtual std::unique_ptr<Component> Clone() const;

    virtual void Awake() {};
    virtual void Start() {};
    virtual void Update() {};
    virtual void Render() const {};
    virtual void AttachGameObject(GameObject* newGameObject);

    bool m_Started;

    friend class Object;
    friend class GameObject;
    friend class Behaviour;
};

} // namespace engine2d