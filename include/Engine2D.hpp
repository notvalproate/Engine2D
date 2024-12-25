#pragma once

#include <filesystem>

#include "Object.hpp"
#include "GameObject.hpp"
#include "Vector2D.hpp"
#include "Transform.hpp"
#include "Behaviour.hpp"

#include "PhysicsHandler.hpp"
#include "SceneHandler.hpp"

#include "components/Components.hpp"

namespace engine2d {

class Engine2D : public Object {
public:
    Engine2D();
    ~Engine2D();

    Engine2D(const Engine2D& other) = delete;
    Engine2D(const Engine2D&& other) = delete;

    Engine2D& operator=(const Engine2D& other) = delete;
    Engine2D& operator=(const Engine2D&& other) = delete;

    void InitGame(const char* title, const std::filesystem::path& iconpath, int windowWidth, int windowHeight);
    void Run();

private:
    virtual void SetupGame() = 0;

    void Update();
    void Render() const;

    bool m_IsRunning;
};

} // namespace engine2d