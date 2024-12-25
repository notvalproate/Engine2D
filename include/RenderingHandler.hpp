#pragma once

#include <vector>

#include "SDL.h"

#include "Vector2D.hpp"
#include "Color.hpp"
#include "Transform.hpp"

namespace engine2d {

class Camera;

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

    struct RenderableColor {
        uint8_t r, g, b, a;
    };

    RenderableColor GetRenderableColor(const Color& color) const;
    SDL_FRect GetSpriteDestRect(const Vector2D& dimensions, const uint16_t pixelsPerUnit, const Transform* transform, const Camera* currentCamera) const;
    void GetFlipAndRotation(const Transform* transform, double& rotation, SDL_RendererFlip& flipFlag) const;
    const std::vector<std::string>& GetAvailableSortingLayers() const;
    unsigned RoundUpToMultipleOfEight(const unsigned v) const;

    SDL_Renderer* m_Renderer;
    std::vector<std::string> m_AvailableSortingLayers;
    mutable std::vector<SDL_Point> m_CirclePointsReserve;

    friend class Object;
    friend class GameObject;
    friend class SceneHandler;
    friend class TextureHandler;
    friend class Engine2D;
};

} // namespace engine2d