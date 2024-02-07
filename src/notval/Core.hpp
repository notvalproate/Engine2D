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

#include <SDL.h>
#include <box2d.h>

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

class Camera;
class SpriteRenderer;
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

private:
    static void CopyBehaviours(GameObject* newGameObject, GameObject* originalGameObject);
    static void CopyComponents(GameObject* newGameObject, GameObject* originalGameObject);
    static void DestroyChildren(GameObject* gameObject);
};


class Vector2D {
public:
    inline constexpr Vector2D() = default;
    inline constexpr Vector2D(double x, double y) : x(x), y(y) {}

    double x{}, y{};

    inline constexpr std::string ToString() const {
        return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    inline double GetMagnitude() const {
        return std::sqrt(x * x + y * y);
    }

    inline constexpr double GetMagnitudeSquared() const {
        return x * x + y * y;
    }

    inline void Normalize() {
        if (x == 0 && y == 0) {
            return;
        }

        double k = std::sqrt(x * x + y * y);
        x /= k;
        y /= k;
    }
    
    inline Vector2D GetNormalized() const {
        Vector2D vec(*this);
        vec.Normalize();

        return vec;
    }

    inline void Scale(const int factor) {
        x *= factor;
        y *= factor;
    }

    static constexpr double epsilon = 1e-5;

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

    inline constexpr double operator*(const Vector2D other) const {
        return x * other.x + y * other.y;
    }

    inline constexpr Vector2D operator*(const double k) const {
        return Vector2D(x * k, y * k);
    }

    friend inline constexpr Vector2D operator*(const double k, const Vector2D vec) {
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

    static inline constexpr double absoluteOf(const double x) {
        return x < 0 ? -x : x;
    }

    inline constexpr bool operator==(const Vector2D other) const {
        return (absoluteOf(x - other.x) < epsilon) && (absoluteOf(y - other.y) < epsilon);
    }

    inline constexpr bool operator!=(const Vector2D other) const {
        return !(*this == other);
    }

    friend inline std::ostream& operator<<(std::ostream& os, const Vector2D vec) {
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

    inline Transform* GetParent() const { return m_Parent; }
    inline std::vector<Transform*> GetChildren() const { return m_Children; }

private:
    explicit Transform(GameObject* gameObject);

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

    void Start();
    void Update();
    void Render() const;

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

            return static_cast<T*>(m_Behaviours.back().get());
        }

        m_ComponentsStagedForAdding.push_back(std::unique_ptr<T>(new T(this)));
        m_ComponentsStagedForAdding.back().get()->Awake();
        m_ComponentsStagedForAdding.back().get()->Start();

        return static_cast<T*>(m_Components.back().get());
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
    Vector2D ScreenToViewportPoint(const Vector2D pos) const;
    Vector2D ScreenToWorldPoint(const Vector2D pos) const;

    Vector2D ViewportToScreenPoint(const Vector2D pos) const;
    Vector2D ViewportToWorldPoint(const Vector2D pos) const;

    Vector2D WorldToScreenPoint(const Vector2D pos) const;
    Vector2D WorldToViewportPoint(const Vector2D pos) const;

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
    void RenderLine(const Vector2D src, const Vector2D dest, const Color color) const;
    void RenderRect(const Vector2D position, const Vector2D dimensions, const Color color) const;
    void RenderSprite(SDL_Texture* texture, const Vector2D dimensions, const uint16_t pixelsPerUnit, const Transform* transform) const;
    void AddSortingLayer(const std::string& name);

private:
    RenderingHandler();

    bool InitRenderer();
    void PresentRenderer();

    SDL_Rect GetSpriteDestRect(const Vector2D dimensions, const uint16_t pixelsPerUnit, const Transform* transform) const;
    void GetFlipAndRotation(const Transform* transform, double& rotation, SDL_RendererFlip& flipFlag) const;
    const std::vector<std::string>& GetAvailableSortingLayers() const;

    SDL_Renderer* m_Renderer;
    std::vector<std::string> m_AvailableSortingLayers;

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

    std::vector<std::unique_ptr<GameObject>> m_SceneGameObjects{};
    std::vector<GameObject*> m_StagedForDestruction{};
    std::vector<SortingLayer> m_SortingLayers;

    Camera* m_CurrentCamera;
    std::vector<Camera*> m_SceneCameras{};

    std::unique_ptr<b2World> m_PhysicsWorld;

    uint32_t m_LatestSceneInstanceID{};
    bool m_Loaded;

    friend class Object;
    friend class Engine2D;
    friend class SceneHandler;
    friend class PhysicsHandler;
    friend class SpriteRenderer;
    friend class BoxCollider;
};


class SceneHandler {
public:
    template<typename T>
    void AddScene(const std::string_view sceneName) {
        AssertSceneIsDerived<T>();
        m_Scenes.push_back(std::unique_ptr<T>(new T(sceneName, Object::RenderingPipeline.GetAvailableSortingLayers())));
    }

    void LoadScene(std::size_t sceneID);
    void LoadScene(const std::string_view sceneName);

    inline Scene* GetCurrentScene() const { return m_CurrentScene; }
    inline Camera* GetCurrentCamera() const { return m_CurrentScene->m_CurrentCamera; }
private:
    SceneHandler();

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

    void WaitForEvent() const;

    inline const SDL_Event& GetCurrentEvent() const { return m_CurrentEvent; }

    int mousePositionX, mousePositionY;
    int mouseScrollDeltaX, mouseScrollDeltaY;
    std::string inputString;

private:
    InputHandler();

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
    
    SDL_Window* m_Window;
    SDL_DisplayMode m_Mode{};
    int m_Width{}, m_Height{};
    int m_InitWidth{}, m_InitHeight{};
    double m_AspectRatio;

    friend class Object;
    friend class Engine2D;
    friend class RenderingHandler;
    friend class TimeHandler;
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


class PhysicsHandler {
public:
    void SetRenderColliders(const bool set);

private:
    PhysicsHandler();

    void RenderColliders() const;

    bool m_RenderSceneColliders;

    friend class Object;
    friend class Scene;
    friend class Engine2D;
    friend class RenderingHandler;
    friend class BoxCollider;
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
