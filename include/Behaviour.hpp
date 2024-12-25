#pragma once

#include "Component.hpp"

namespace engine2d {

struct Collision;

class Behaviour : public Component {
public:
    bool enabled;
    bool isActiveAndEnabled;
    std::string* name;

protected:
    explicit Behaviour(GameObject* gameObject);

private:
    std::unique_ptr<Component> Clone() const override;

    virtual void OnCollisionEnter(const Collision&) {};
    virtual void OnCollisionStay(const Collision&) {};
    virtual void OnCollisionExit(const Collision&) {};

    virtual void OnTriggerEnter(const Collision&) {};
    virtual void OnTriggerStay(const Collision&) {};
    virtual void OnTriggerExit(const Collision&) {};

    virtual void LateUpdate() {};
    virtual void OnDestroy() {};

    void Render() const override final {};
    void AttachGameObject(GameObject* newGameObject) override final;

    friend class Object;
    friend class GameObject;
};

} // namespace engine2d