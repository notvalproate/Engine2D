#pragma once

#include <filesystem>
#include <string>

#include <SDL.h>

namespace engine2d {

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

    bool InitScreen(const std::string& title, const std::filesystem::path& iconpath, const int windowWidth, const int windowHeight);
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

} // namespace engine2d