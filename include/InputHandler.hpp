#pragma once

#include <string>

#include <SDL.h>

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