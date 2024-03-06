#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <chrono>
#include <unordered_map>

#include <SDL.h>
#include <box2d.h>

#include "MathModule.hpp"

class Object;
class Vector2D;
class Component;
class Behaviour;
class Transform;
class GameObject;
class Scene;
class Engine2D;

class InputHandler;
class SceneHandler;
class ScreenHandler;
class RenderingHandler;
class TextureHandler;
class TimeHandler;
class PhysicsHandler;
class Cursor;
class CursorHandler;

class Camera;
class SpriteRenderer;
class RigidBody;
class Collider;
class BoxCollider;


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

    static InputHandler Input;
    static SceneHandler SceneManager;
    static ScreenHandler Screen;
    static RenderingHandler RenderingPipeline;
    static TextureHandler TextureManager;
    static TimeHandler Time;
    static PhysicsHandler Physics;
    static CursorHandler Cursor;
    static MathModule Math;

private:
    static void CopyBehaviours(GameObject* newGameObject, GameObject* originalGameObject);
    static void CopyComponents(GameObject* newGameObject, GameObject* originalGameObject);
    static void DestroyChildren(GameObject* gameObject);
};


class Vector2D {
public:
    explicit inline constexpr Vector2D() = default;
    explicit inline constexpr Vector2D(double x, double y) : x(x), y(y) {}
    explicit inline Vector2D(const b2Vec2 box2dVec) : x(box2dVec.x), y(box2dVec.y) { }

    double x{}, y{};

    inline Vector2D& operator=(const b2Vec2 box2dVec) {
        x = box2dVec.x;
        y = box2dVec.y;
        return *this;
    }

    inline constexpr std::string ToString() const {
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    inline double GetMagnitude() const {
        return std::sqrt(x * x + y * y);
    }

    inline constexpr double GetMagnitudeSquared() const {
        return x * x + y * y;
    }

    inline Vector2D& Normalize() {
        if (x == 0 && y == 0) {
            return *this;
        }

        double k = std::sqrt(x * x + y * y);
        x /= k;
        y /= k;

        return *this;
    }
    
    inline Vector2D GetNormalized() const {
        return Vector2D(*this).Normalize();
    }

    inline constexpr Vector2D& Scale(const int factor) {
        x *= factor;
        y *= factor;

        return *this;
    }

    Vector2D& Rotate(const double angle) {
        const double angleRadians = angle * M_PI / 180.0;

        const double sinTheta = sin(angleRadians);
        const double cosTheta = cos(angleRadians);
        
        const double newX = x * cosTheta + y * sinTheta;
        const double newY = y * cosTheta - x * sinTheta;

        x = newX;
        y = newY;

        return *this;
    }
    
    inline Vector2D& RotateAround(const Vector2D& point, const double angle) {
        *this -= point;
        Rotate(angle);
        *this += point;

        return *this;
    }

    inline constexpr bool Equals(const Vector2D& other, const double epsil) const {
        return (Object::Math.Abs(x - other.x) < epsil) && (Object::Math.Abs(y - other.y) < epsil);
    }

    static inline constexpr double Cross(const Vector2D& v1, const Vector2D& v2) {
        return (v1.x * v2.y) - (v1.y * v2.x); 
    }

    static inline double Distance(const Vector2D& v1, const Vector2D& v2) {
        double dx = v2.x - v1.x;
        double dy = v2.y - v1.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    static inline constexpr double SquaredDistance(const Vector2D& v1, const Vector2D& v2) {
        double dx = v2.x - v1.x;
        double dy = v2.y - v1.y;
        return dx * dx + dy * dy;
    }

    static double Angle(const Vector2D& from, const Vector2D& to) {
        if (from == to) {
            return 0.0;
        }

        if (from == -to) {
            return 180.0;
        }

        double dot = from * to;

        if (dot == 0) {
            return 90.0;
        }

        double magnitudeFromTo = from.GetMagnitude() * to.GetMagnitude();

        if (magnitudeFromTo == 0.0) {
            return 0.0;
        }

        double cosAngle = std::clamp(dot / magnitudeFromTo, -1.0, 1.0);
        double angleRadians = std::acos(cosAngle);
        double angleDegrees = angleRadians * (180.0 / M_PI);

        return angleDegrees;
    }

    static double SignedAngle(const Vector2D& from, const Vector2D& to) {
        double angle = Angle(from, to);
        double crossProduct = Cross(from, to);
        angle *= Object::Math.Sign(-crossProduct);

        return angle;
    }

    static inline Vector2D ClampMagnitude(Vector2D vec, const double maxLength) {
        double currentMagnitude = vec.GetMagnitude();

        if (currentMagnitude > maxLength && currentMagnitude > 0.0) {
            double scaleFactor = maxLength / currentMagnitude;
            vec.x *= scaleFactor;
            vec.y *= scaleFactor;
        }

        return vec;
    }

    static inline constexpr Vector2D Min(const Vector2D& v1, const Vector2D& v2) {
        return Vector2D(std::min(v1.x, v2.x), std::min(v1.y, v2.y));
    }
    
    static inline constexpr Vector2D Max(const Vector2D& v1, const Vector2D& v2) {
        return Vector2D(std::max(v1.x, v2.x), std::max(v1.y, v2.y));
    }

    static inline constexpr Vector2D Perpendicular(const Vector2D& vec) {
        return Vector2D(vec.y, -(vec.x));
    }

    static inline constexpr Vector2D Reflect(const Vector2D& inDirection, const Vector2D& inNormal) {
        double dotProduct = 2.0 * (inDirection * inNormal);
        double magnitudeSquared = inNormal.GetMagnitudeSquared();
        return inDirection - ((dotProduct / magnitudeSquared) * inNormal);
    }

    static inline constexpr Vector2D Lerp(const Vector2D& v1, const Vector2D& v2, double t) {
        t = std::clamp(t, 0.0, 1.0);
        return Vector2D(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t);
    }
    
    static inline constexpr Vector2D LerpUnclamped(const Vector2D& v1, const Vector2D& v2, double t) {
        return Vector2D(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t);
    }

    static Vector2D MoveTowards(const Vector2D& current, const Vector2D& target, double maxDistanceDelta) {
        Vector2D vectorToTarget = target - current;

        double distanceSquared = vectorToTarget.GetMagnitudeSquared();

        if (distanceSquared == 0.0 || (maxDistanceDelta >= 0 && distanceSquared <= maxDistanceDelta * maxDistanceDelta)) {
            return target;
        }

        double distance = std::sqrt(distanceSquared);

        return current + ((vectorToTarget * maxDistanceDelta) / distance);
    }

    static Vector2D SmoothDamp(
        const Vector2D& current,
        const Vector2D& target,
        Vector2D& currentVelocity,
        double smoothTime,
        double deltaTime,
        double maxSpeed = std::numeric_limits<double>::infinity()
        )
    {
        if (current == target) {
            currentVelocity = Vector2D::zero;
            return target;
        }

        float omega = 2.0 / smoothTime;
        float x = omega * deltaTime;
        float exp = 1.0 / (1.0 + x + 0.48 * x * x + 0.235 * x * x * x);

        Vector2D change = current - target;
        Vector2D tempTarget = target + (change + currentVelocity * smoothTime) * deltaTime;
        Vector2D tempVelocity = (currentVelocity - change * omega) * deltaTime;
        Vector2D newVelocity = (currentVelocity - tempVelocity * omega) * exp;

        Vector2D output = target + (change + tempVelocity) * exp;
        if (maxSpeed != std::numeric_limits<double>::infinity()) {
            Vector2D outputDir = output - target;
            double outputSpeed = outputDir.GetMagnitude();
            if (outputSpeed > maxSpeed) {
                outputDir = outputDir.GetNormalized() * maxSpeed;
                output = target + outputDir;
                newVelocity = (output - target) / deltaTime;
            }
        }

        currentVelocity = newVelocity;

        return output;
    }

    static constexpr double epsilon = 1e-4;

    inline constexpr Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    inline constexpr Vector2D& operator+=(const Vector2D& other) {
        x += other.x;
        y += other.y;

        return *this;
    }

    inline constexpr Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    inline constexpr Vector2D& operator-=(const Vector2D& other) {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    inline constexpr Vector2D operator-() const {
        return Vector2D(-x, -y);
    }

    inline constexpr double operator*(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    inline constexpr Vector2D operator*(const double k) const {
        return Vector2D(x * k, y * k);
    }

    friend inline constexpr Vector2D operator*(const double k, const Vector2D& vec) {
        return vec * k;
    }

    inline constexpr Vector2D& operator*=(const double k) {
        x *= k;
        y *= k;
        return *this;
    }

    inline constexpr Vector2D operator/(const double k) const {
        return Vector2D(x / k, y / k);
    }

    inline constexpr Vector2D& operator/=(const double k) {
        x /= k;
        y /= k;
        return *this;
    }

    inline constexpr bool operator==(const Vector2D& other) const {
        return (Object::Math.Abs(x - other.x) < epsilon) && (Object::Math.Abs(y - other.y) < epsilon);
    }

    inline constexpr bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }

    friend inline std::ostream& operator<<(std::ostream& os, const Vector2D& vec) {
        os << vec.ToString();
        return os;
    }

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


class Behaviour : public Component {
public:
    bool enabled;
    bool isActiveAndEnabled;
    std::string* name;

protected:
    explicit Behaviour(GameObject* gameObject);

private:
    std::unique_ptr<Component> Clone() const override;

    virtual void LateUpdate() {};
    virtual void OnDestroy() {};

    void Render() const override final { };
    void AttachGameObject(GameObject* newGameObject) override final;

    friend class Object;
    friend class GameObject;
};


class Transform final {
public:
    Vector2D position{};
    double rotation{};
    Vector2D scale{Vector2D::one};

    GameObject* gameObject;

    std::size_t childCount{};

    std::string* tag;
    std::string* name;

    void Translate(const Vector2D& translation);
    void Rotate(const double angle);
    void RotateAround(const Vector2D& point, const double angle);
    void Scale(const Vector2D& scaleSet);
    void SetScale(const Vector2D& scaleSet);

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

    void SetParent(Transform* parentTransform, bool instantiateInWorldSpace = false);
    void SetParent(Transform& parentTransform, bool instantiateInWorldSpace = false);
    void SetParent(GameObject* parentGo, bool instantiateInWorldSpace = false);

    inline Transform* GetParent() const { return m_Parent; }
    inline std::vector<Transform*> GetChildren() const { return m_Children; }

private:
    explicit Transform(GameObject* gameObject);

    void AddToRotation(const double angle);

    std::vector<Transform*> m_Children{};
    Transform* m_Parent;

    friend class Object;
    friend class GameObject;
};


class GameObject final : public Object {
public:
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

    std::vector<std::unique_ptr<Component>> m_ComponentsStagedForAdding{};
    std::vector<std::unique_ptr<Behaviour>> m_BehavioursStagedForAdding{};
    std::vector<std::unique_ptr<Component>> m_Components{};
    std::vector<std::unique_ptr<Behaviour>> m_Behaviours{};
    std::vector<Component*> m_ComponentsStagedForDestruction{};
    std::vector<Behaviour*> m_BehavioursStagedForDestruction{};

    friend class Object;
    friend class Scene;
    friend class RenderingHandler;
};


class Camera final : public Component {
public:
    ~Camera();

    Vector2D ScreenToViewportPoint(const Vector2D& pos) const;
    Vector2D ScreenToWorldPoint(const Vector2D& pos) const;

    Vector2D ViewportToScreenPoint(const Vector2D& pos) const;
    Vector2D ViewportToWorldPoint(const Vector2D& pos) const;

    Vector2D WorldToScreenPoint(const Vector2D& pos) const;
    Vector2D WorldToViewportPoint(const Vector2D& pos) const;

    double GetPixelsPerUnit() const;

private:
    explicit Camera(GameObject* gameObject);

    std::unique_ptr<Component> Clone() const;

    Vector2D GetUnitsOnScreen() const;

    static constexpr uint8_t defaultUnitsY = 10;

    friend class GameObject;
};


struct SortingLayer {
    SortingLayer(const std::string_view layerName) : name(layerName), m_GameObjectsInLayer({}) {}

    std::string name;
    std::vector<GameObject*> m_GameObjectsInLayer;
};

struct Color {
    explicit Color() = default;
    explicit Color(const uint8_t red, const uint8_t green, const uint8_t blue) : r(red), g(green), b(blue), a(255) { }
    explicit Color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha) : r(red), g(green), b(blue), a(alpha) { }

    uint8_t r{}, g{}, b{}, a{};
};

class RenderingHandler {
public:
    void SetRendererVsync(const bool set);
    void RenderPoint(const Vector2D& point, const uint8_t width, const Color color) const;
    void RenderLine(const Vector2D& src, const Vector2D& dest, const Color color) const;
    void RenderRect(const Vector2D& position, const Vector2D& dimensions, const Color color) const;
    void RenderCircle(const Vector2D& position, const double radius, const Color color) const;
    void RenderSprite(SDL_Texture* texture, const Vector2D& dimensions, const uint16_t pixelsPerUnit, const Transform* transform) const;
    void AddSortingLayer(const std::string& name);

private:
    RenderingHandler();

    bool InitRenderer();
    void PresentRenderer();
    void DestroyRenderer();

    SDL_FRect GetSpriteDestRect(const Vector2D& dimensions, const uint16_t pixelsPerUnit, const Transform* transform, const Camera* currentCamera) const;
    void GetFlipAndRotation(const Transform* transform, double& rotation, SDL_RendererFlip& flipFlag) const;
    const std::vector<std::string>& GetAvailableSortingLayers() const;
    int RoundUpToMultipleOfEight(const int v) const;

    SDL_Renderer* m_Renderer;
    std::vector<std::string> m_AvailableSortingLayers;
    mutable std::vector<SDL_Point> m_CirclePointsReserve;

    friend class Object;
    friend class GameObject;
    friend class SceneHandler;
    friend class TextureHandler;
    friend class Engine2D;
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
    std::unordered_map<b2Fixture*, Collider*> m_FixtureColliderMap{};

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


class SceneHandler {
public:
    template<typename T>
    void AddScene(const std::string_view sceneName) {
        AssertSceneIsDerived<T>();
        m_Scenes.push_back(std::unique_ptr<T>(new T(sceneName, Object::RenderingPipeline.GetAvailableSortingLayers())));
    }

    // WORRY ABOUT HOW ADDITIVE SCENE LOADING IS CURRENLTY THE ONLY WAY. FIND OUT WAY TO NON ADDITIVELY LOAD / UNLOAD THE CURRENTSCENE
    void LoadScene(std::size_t sceneID);
    void LoadScene(const std::string_view sceneName);

    inline Scene* GetCurrentScene() const { return m_CurrentScene; }
    inline Camera* GetCurrentCamera() const { return m_CurrentScene->m_CurrentCamera; }
private:
    SceneHandler();

    void DestroyScenes();

    std::vector<std::unique_ptr<Scene>> m_Scenes{};
    Scene* m_CurrentScene;

    template<typename T>
    static void AssertSceneIsDerived() {
        static_assert(
            std::is_base_of<Scene, T>::value,
            "Scene provided not derived from Scene Class"
        );
    }

    friend class Object;
    friend class Engine2D;
    friend class RenderingHandler;
};


class InputHandler {
public:
    bool GetKey(const SDL_Scancode scanCode) const;
    bool GetKeyUp(const SDL_Scancode scanCode) const;
    bool GetKeyDown(const SDL_Scancode scanCode) const;

    bool GetMouseButton(const uint8_t buttonCode) const;
    bool GetMouseButtonUp(const uint8_t buttonCode) const;
    bool GetMouseButtonDown(const uint8_t buttonCode) const;

    short GetAxisRaw(const std::string& axis) const;

    void WaitForEvent() const;

    inline const SDL_Event& GetCurrentEvent() const { return m_CurrentEvent; }

    int mousePositionX, mousePositionY;
    int mouseRelX, mouseRelY;
    int mouseScrollDeltaX, mouseScrollDeltaY;
    std::string inputString;

private:
    InputHandler();

    void InitInput();

    void PollEvents();
    void SetProperties();

    SDL_Event m_CurrentEvent;
    const uint8_t* m_KeyboardState;

    friend class Object;
    friend class Engine2D;
};


class ScreenHandler {
public:
    void ToggleFullscreen();
    void SetResolution(const int w, const int h);
    void SetToInitialResolution();
    void SetToDisplayResolution();

    inline unsigned int GetScreenWidth() const { return m_Width; }
    inline unsigned int GetScreenHeight() const { return m_Height; }
    inline double GetAspectRatio() const { return m_AspectRatio; }
    inline bool InFocus() const { return (SDL_GetWindowFlags(m_Window) & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS)); };
    inline bool IsFullscreen() const { return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_FULLSCREEN; }

private:
    ScreenHandler();

    bool InitScreen(const char* title, const char* iconpath, const int windowWidth, const int windowHeight);
    void DestroyScreen();
    
    SDL_Window* m_Window;
    SDL_DisplayMode m_Mode{};
    int m_Width{}, m_Height{};
    int m_InitWidth{}, m_InitHeight{};
    double m_AspectRatio;

    friend class Object;
    friend class Engine2D;
    friend class RenderingHandler;
    friend class TimeHandler;
    friend class CursorHandler;
};


class TextureHandler {
public:
    SDL_Texture* LoadTexture(const char* texpath) const;
    SDL_Point GetTextureSize(SDL_Texture* texture) const;

private:
    TextureHandler() = default;

    friend class Object;
};


class TimeHandler {
public:
    inline float GetFixedDeltaTime() const { return m_FixedDeltaTime; }
    inline float GetDeltaTime() const { return m_DeltaTime; }
    inline unsigned int GetFixedFramerate() const { return static_cast<unsigned int>(m_FixedFramerate); }
    inline unsigned int GetFramerate() const { return static_cast<unsigned int>(1 / m_DeltaTime); }
    inline uint32_t GetFrameCount() const { return m_FrameCount; }

private:
    TimeHandler();

    void InitTime();
    void UpdateDeltaTime();

    unsigned int m_FixedFramerate;
    float m_FixedDeltaTime;
    float m_DeltaTime;
    uint32_t m_FrameCount;

    std::chrono::system_clock::time_point m_FrameStart;

    friend class Object;
    friend class Engine2D;
};

struct RayCastHit {
    bool hit{false};
    Vector2D point;
    Vector2D normal;
    float fraction{0.0f};
    float distance{0.0f};
    Collider* collider{nullptr};

    RayCastHit() {}
};

class PhysicsHandler {
public:
    void SetRenderColliders(const bool set);
    RayCastHit RayCast(const Vector2D& origin, const Vector2D& direction, float distance) const;

    void SetGravity(const Vector2D& gravity) const;
    Vector2D GetGravity() const;
private:
    PhysicsHandler();

    void RenderColliders() const;

    bool m_RenderSceneColliders;

    class RayCastCallback : public b2RayCastCallback {
    public:
        float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
        inline bool DidHit() const { return m_result.hit; }
        inline RayCastHit& GetResult() { return m_result; }
        inline b2Fixture* GetFixture() { return fixtureHit; }

    private:
        RayCastHit m_result;
        b2Fixture* fixtureHit;
    };

    friend class Object;
    friend class Scene;
    friend class Engine2D;
    friend class RenderingHandler;
    friend class BoxCollider;
    friend class RayCastCallback;
};

enum class CursorLockMode : uint8 {
    None = 0,
    Locked = 1,
    Confined = 2,
    CursorLockModeCount = 3
};

enum class SystemCursorType : uint8 {
    Arrow = 0,
    IBeam = 1,
    Wait = 2,
    Crosshair = 3,
    WaitArrow = 4,
    NWSEArrow = 5,
    NESWArrow = 6,
    EWArrow = 7,
    NSArrow = 8,
    AllArrow = 9,
    No = 10,
    Hand = 11,
    SystemCursorCount = 12
};

class CustomCursor {
public:
    CustomCursor(const char* cursorPath, const Vector2D& hotspot);
    ~CustomCursor();
private:
    SDL_Cursor* m_Cursor;

    friend class CursorHandler;
};

class CursorHandler {
public:
    void SetVisibility(bool visible);
    void SetLockState(CursorLockMode lockType);
    void SetCursor(SystemCursorType cursor);
    void SetCursor(const CustomCursor& cursor);

    bool GetVisibility() const;
    CursorLockMode GetLockState() const;
private:
    CursorHandler();

    void InitSystemCursors();
    void DestroySystemCursors();

    bool m_Visibility;
    CursorLockMode m_LockState;
    std::vector<SDL_Cursor*> m_SystemCursors;

    friend class Object;
    friend class Engine2D;
};

class Engine2D : public Object {
public:
    Engine2D();
    ~Engine2D();

    Engine2D(const Engine2D& other) = delete;
    Engine2D(const Engine2D&& other) = delete;

    Engine2D& operator=(const Engine2D& other) = delete;
    Engine2D& operator=(const Engine2D&& other) = delete;

    void InitGame(const char* title, const char* iconpath, int windowWidth, int windowHeight);
    void Run();

private:
    virtual void SetupGame() = 0;

    void Update();
    void Render() const;

    bool m_IsRunning;
};
