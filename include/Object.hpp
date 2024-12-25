#pragma once

#include "Engine2DExport.hpp"

#include "InputHandler.hpp"
#include "RenderingHandler.hpp"
#include "ScreenHandler.hpp"
#include "TextureHandler.hpp"
#include "TimeHandler.hpp"
#include "CursorHandler.hpp"
#include "MathModule.hpp"

namespace engine2d {

class GameObject;
class Vector2D;
class Transform;
class Behaviour;
class Component;

class SceneHandler;
class PhysicsHandler;

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

    static ENGINE2D_API InputHandler Input;
    static ENGINE2D_API SceneHandler SceneManager;
    static ENGINE2D_API ScreenHandler Screen;
    static ENGINE2D_API RenderingHandler RenderingPipeline;
    static ENGINE2D_API TextureHandler TextureManager;
    static ENGINE2D_API TimeHandler Time;
    static ENGINE2D_API PhysicsHandler Physics;
    static ENGINE2D_API CursorHandler Cursor;
    static ENGINE2D_API MathModule Math;
private:
    static void CopyBehaviours(GameObject* newGameObject, GameObject* originalGameObject);
    static void CopyComponents(GameObject* newGameObject, GameObject* originalGameObject);
    static void DestroyChildren(GameObject* gameObject);
};

} // namespace engine2d